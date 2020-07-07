#include "FileIO.hpp"
#include <map>
#include <list>
#include <string.h>

namespace LYW_CODE
{
    template <typename IOType = FileIO>
    class FileStorage
    {
        public:

        /*数据结构*/
        /*数据块索引文件信息结果 存放于索引文件 以及索引文件索引查找*/
        typedef struct _IndexFileBlockIndexInfo
        {
            /*数据在数据文件中开始的位置*/
            unsigned long beginIndex;
            /*数据块的大小*/
            unsigned long blockSize;
            /*索引块在索引文件的位置 为0 标识索引块无效*/
            unsigned long pos;
        }TIndexFileBlockIndexInfo, *PIndexFileBlockIndexInfo;

        /*索引文件 文件头结构*/
        typedef struct _IndexFileHead
        {
            /*索引块结束的位置*/
            unsigned long EndPos;

            /*数据文件结束位置*/
            unsigned long DataEndPos;
        }TIndexFileHead, *PIndexFileHead;

        /*索引文件文件块*/
        typedef struct  _IndexFileBlock
        {
            union 
            {
                TIndexFileBlockIndexInfo IndexBlock;
                TIndexFileHead HeadBlock;
            };
        }TIndexFileBlock, * PIndexFileBlock;


        public:

            
            FileStorage()
            {
                m_IndexFile = new IOType;
                m_DataFile = new IOType;

                m_IndexFileName = "./FileStorage.index";
                m_DataFileName = "./FileStorage.data";

                m_FreeIndexBlockCache.clear();
            }

            ~FileStorage()
            {
                if (m_IndexFile != NULL)
                {
                    m_IndexFile->close();
                    delete m_IndexFile;
                    m_IndexFile = NULL;
                }

                if (m_DataFile != NULL)
                {
                    m_DataFile->close();
                    delete m_DataFile;
                    m_DataFile = NULL;
                }
            }


            
           /**
            * @brief 
            *
            * @param handle
            * @param buf
            * @param sizeOfbuf
            *
            * @return 
            */
            int read(void * handle, void *buf, unsigned long sizeOfbuf)
            {
                if (handle == NULL || buf == NULL)
                {
                    return -1;
                }

                if (!IsInit())
                {
                    return -1;
                }

                PIndexFileBlock pIndexBlock = (PIndexFileBlock)handle;

                if (pIndexBlock->IndexBlock.blockSize > sizeOfbuf)
                {
                    return false;
                }

                m_DataFile->lseek(pIndexBlock->IndexBlock.beginIndex,SEEK_SET);
                return m_DataFile->read(buf, sizeOfbuf, pIndexBlock->IndexBlock.beginIndex);
            }


            /**
             * @brief 
             *
             * @param handle
             * @param buf
             * @param lenOfBuf
             *
             * @return 
             */
            int write(void * handle, void * buf, unsigned long lenOfBuf)
            {
                if (handle == NULL || buf == NULL)
                {
                    return -1;
                }

                if (!IsInit())
                {
                    return -1;
                }

                PIndexFileBlock pIndexBlock = (PIndexFileBlock)handle;
                m_DataFile->lseek(pIndexBlock->IndexBlock.beginIndex,SEEK_SET);
                return m_DataFile->write(buf,lenOfBuf);
            }
            
            
            /**
             * @brief           获取一块文件存储区域,切记勿修改存储指针内容
             *
             * @param size      存储区域大小写
             *
             * @return          存储信息指针
             */
            void * allocate(unsigned long size)
            {
                PIndexFileBlock pIndexBlock = NULL;
                if (size == 0)
                {
                    return NULL;
                }

                if (!IsInit())
                {
                    return NULL;
                }

                pIndexBlock = new TIndexFileBlock;
                
                /*空闲块中查找*/
                for (auto &p : m_BeginIndexMap)
                {
                    if (p.second.IndexBlock.blockSize >= size)
                    {
                        memcpy(pIndexBlock, &p.second, sizeof(TIndexFileBlock));

                        FreeIndexBlock(p.second);

                        m_BeginIndexMap.erase(pIndexBlock->IndexBlock.beginIndex);
                        m_EndIndexMap.erase(pIndexBlock->IndexBlock.beginIndex + pIndexBlock->IndexBlock.blockSize);

                        return pIndexBlock;
                    }
                }

                pIndexBlock->IndexBlock.pos = 0;
                pIndexBlock->IndexBlock.beginIndex = m_IndexFileHead.HeadBlock.DataEndPos;
                pIndexBlock->IndexBlock.blockSize = size;

                m_IndexFileHead.HeadBlock.DataEndPos += size;
                SyncToIndexFile(0, m_IndexFileHead);

                return pIndexBlock;
            }

            void free(void * p )
            {
                PIndexFileBlock indexBlock = (PIndexFileBlock)p;
                unsigned long beginIndex = 0;
                unsigned long endIndex = 0;
                unsigned long pos;

                if (indexBlock == NULL)
                {
                    return;
                }
                
                if (!IsInit())
                {
                    return;
                }

                /*当前归还块是否需要合并 不合并则需要存储当前索引块*/ 
                /*往前合并 当前块的首 是否为其他块的尾*/
                if (m_EndIndexMap.find(indexBlock->IndexBlock.beginIndex) != m_EndIndexMap.end())
                {
                    endIndex = indexBlock->IndexBlock.beginIndex;

                    pos = m_EndIndexMap[indexBlock->IndexBlock.beginIndex].IndexBlock.pos;
                    indexBlock->IndexBlock.beginIndex = m_EndIndexMap[indexBlock->IndexBlock.beginIndex].IndexBlock.beginIndex;
                    indexBlock->IndexBlock.blockSize += m_EndIndexMap[indexBlock->IndexBlock.beginIndex].IndexBlock.blockSize;

                    m_EndIndexMap.erase(endIndex);
                    m_BeginIndexMap.erase(indexBlock->IndexBlock.beginIndex);

                    indexBlock->IndexBlock.pos = pos;
                    FreeIndexBlock(*indexBlock);

                    indexBlock->IndexBlock.pos = 0;
                    SyncToIndexFile(pos, *indexBlock);
                }

                /*往后合并*/
                beginIndex = indexBlock->IndexBlock.beginIndex + indexBlock->IndexBlock.blockSize;
                if (m_BeginIndexMap.find(beginIndex) != m_BeginIndexMap.end())
                {
                    indexBlock->IndexBlock.blockSize += m_BeginIndexMap[beginIndex].IndexBlock.blockSize;

                    pos = m_BeginIndexMap[beginIndex].IndexBlock.pos;

                    m_BeginIndexMap.erase(beginIndex);
                    m_EndIndexMap.erase(indexBlock->IndexBlock.beginIndex + indexBlock->IndexBlock.blockSize);

                    indexBlock->IndexBlock.pos = pos;
                    FreeIndexBlock(*indexBlock);

                    indexBlock->IndexBlock.pos = 0;
                    SyncToIndexFile(pos, *indexBlock);
                }

                /*record*/
                GetIndexBlock(indexBlock);
                m_FreeIndexBlockCache.push_back(*indexBlock);
                //delete indexBlock;
            }

        private:
            bool SyncToIndexFile(unsigned long pos, const TIndexFileBlock & indexBlock)
            {
                if (m_IndexFile == NULL || !m_IndexFile->IsOpen())
                {
                    return false;
                }

                if (m_IndexFile->lseek(pos,SEEK_SET) < 0)
                {
                    return false;
                }

                if (m_IndexFile->write(&indexBlock, sizeof(TIndexFileBlock)) < 0)
                {
                    return false;
                }
                return true;
            }





            bool GetIndexBlock(PIndexFileBlock indexBlock)
            {
                if (m_IndexFile == NULL || !m_IndexFile->IsOpen())
                {
                    return false;
                }

                if (!m_FreeIndexBlockCache.empty())
                {
                    memcpy(indexBlock, &(m_FreeIndexBlockCache.front()), sizeof(TIndexFileBlock));
                    m_FreeIndexBlockCache.pop_front();
                    return true;
                }
                else
                {
                    indexBlock->IndexBlock.pos = m_IndexFileHead.HeadBlock.EndPos;

                    m_IndexFileHead.HeadBlock.EndPos += sizeof(TIndexFileBlock);
                    SyncToIndexFile(0,m_IndexFileHead);
                    return true;
                }
            }

            bool FreeIndexBlock(TIndexFileBlock indexBlock)
            {
                if (m_IndexFile == NULL || !m_IndexFile->IsOpen())
                {
                    return false;
                }
                
                /*文件尾 则缩短文件*/
                if (indexBlock.IndexBlock.pos + sizeof(TIndexFileBlock) == m_IndexFileHead.HeadBlock.EndPos)
                {
                    m_IndexFileHead.HeadBlock.EndPos = indexBlock.IndexBlock.pos;
                    return SyncToIndexFile(0, m_IndexFileHead);
                }
                else
                {
                    m_FreeIndexBlockCache.push_back(indexBlock);
                    int pos = indexBlock.IndexBlock.pos;
                    indexBlock.IndexBlock.pos = 0;
                    return SyncToIndexFile(pos,indexBlock);
                }
            }

            bool InitIndexFile()
            {
                m_FreeIndexBlockCache.clear();
                memset(&m_IndexFileHead, 0x00, sizeof(TIndexFileBlock));
                m_IndexFileHead.HeadBlock.EndPos = sizeof(TIndexFileBlock);
                m_IndexFileHead.HeadBlock.DataEndPos = 0;
                return SyncToIndexFile(0, m_IndexFileHead);
            }
 

            bool IsInit()
            {
                TIndexFileBlock tmpIndexBlock;
                int FileSize = 0;
                /*数据文件是否就绪*/ 
                if (!m_DataFile->IsOpen())
                {
                    if (m_DataFile->open(m_DataFileName) < 0)
                    {
                        return false;
                    }
                }
                
                /*检测索引文件是否就绪 未就绪读取索引文件 并加载内存*/
                if (!m_IndexFile->IsOpen())
                {
                    if (m_IndexFile->open(m_IndexFileName) < 0)
                    {
                        return false;
                    }

                    FileSize = m_IndexFile->size();

                    /*new file*/
                    if (FileSize < sizeof(TIndexFileBlock))
                    {
                        if (InitIndexFile())
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }

                    if (m_IndexFile->lseek(0,SEEK_SET) < 0)
                    {
                        return false;
                    }



                    /*加载内存 此处的块合并逻辑较为简单粗暴 最大的优化点;读取空闲索引块至列表*/
                    /*读取头*/
                    if (m_IndexFile->read(&m_IndexFileHead,sizeof(TIndexFileBlock),sizeof(TIndexFileBlock)) != sizeof(TIndexFileBlock))
                    {
                        return false;
                    }

                    /*头文件 offset 读取所有索引块*/
                    if (FileSize < m_IndexFileHead.HeadBlock.EndPos)
                    {
                        if (InitIndexFile())
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }

                    FileSize = m_IndexFileHead.HeadBlock.EndPos;
                    FileSize -= sizeof(TIndexFileBlock);

                    while (FileSize > 0)
                    {
                        m_IndexFile->read(&tmpIndexBlock, sizeof(TIndexFileBlock), sizeof(TIndexFileBlock));
                        FileSize -= sizeof(TIndexFileBlock);

                        if (tmpIndexBlock.IndexBlock.pos == 0)
                        {
                            m_FreeIndexBlockCache.push_back(tmpIndexBlock);
                        }
                        else
                        {
                            m_BeginIndexMap[tmpIndexBlock.IndexBlock.beginIndex] = tmpIndexBlock;
                            m_EndIndexMap[tmpIndexBlock.IndexBlock.beginIndex + tmpIndexBlock.IndexBlock.blockSize] = tmpIndexBlock;

                        }
                    }

                    return true;
                }
                return true;
            }

        private:

            BaseFileIO * m_IndexFile;
            std::string m_IndexFileName;

            BaseFileIO * m_DataFile;
            std::string m_DataFileName;
            /*空闲的索引块信息 便于索引块的快速存取*/
            std::list <TIndexFileBlock> m_FreeIndexBlockCache;
            TIndexFileBlock m_IndexFileHead;

            std::map<unsigned int, TIndexFileBlock> m_BeginIndexMap;
            std::map<unsigned int, TIndexFileBlock> m_EndIndexMap;
            
    };
}

