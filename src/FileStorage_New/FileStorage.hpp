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
            /*索引块在索引文件的位置*/
            unsigned long pos;
            /*0 空闲 1 数据资源释放 2 数据资源占用*/
            unsigned long Tag;
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


            TIndexFileBlock TranslateHandle(void * handle)
            {
                TIndexFileBlock Handle;
                memset(&Handle, handle, sizeof(TIndexFileBlock));
                return Handle;
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
            int read(unsigned int handle, void *buf, unsigned long sizeOfbuf)
            {
                if (handle == 0 || buf == NULL)
                {
                    return -1;
                }

                if (!IsInit())
                {
                    return -1;
                }

                TIndexFileBlock IndexBlock;
                if (!ReadIndexBlockByPos(handle, &IndexBlock))
                {
                    return -1;
                }

                if (IndexBlock.IndexBlock.blockSize > sizeOfbuf)
                {
                    return false;
                }

                m_DataFile->lseek(IndexBlock.IndexBlock.beginIndex,SEEK_SET);
                return m_DataFile->read(buf, sizeOfbuf, IndexBlock.IndexBlock.blockSize);
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
            int write(unsigned int handle, void * buf, unsigned long lenOfBuf)
            {
                if (handle == 0 || buf == NULL)
                {
                    return -1;
                }

                if (!IsInit())
                {
                    return -1;
                }

                TIndexFileBlock IndexBlock;
                if (!ReadIndexBlockByPos(handle,&IndexBlock))
                {
                    return 0;
                }
                m_DataFile->lseek(IndexBlock.IndexBlock.beginIndex,SEEK_SET);
                return m_DataFile->write(buf,lenOfBuf);
            }
            
            
            /**
             * @brief           获取一块文件存储区域,切记勿修改存储指针内容
             *
             * @param size      存储区域大小写
             *
             * @return          存储信息指针
             */
            unsigned int allocate(unsigned long size)
            {
                PIndexFileBlock pIndexBlock = NULL;
                if (size == 0)
                {
                    return 0;
                }

                if (!IsInit())
                {
                    return 0;
                }

                TIndexFileBlock IndexBlock;
                
                /*空闲块中查找*/
                for (auto &p : m_BeginIndexMap)
                {
                    if (p.second.IndexBlock.blockSize >= size)
                    {
                        memcpy(&IndexBlock, &p.second, sizeof(TIndexFileBlock));
                        m_BeginIndexMap.erase(IndexBlock.IndexBlock.beginIndex);
                        m_EndIndexMap.erase(IndexBlock.IndexBlock.beginIndex + IndexBlock.IndexBlock.blockSize);
                        IndexBlock.IndexBlock.Tag = 2;
                        SyncToIndexFile(IndexBlock.IndexBlock.pos, IndexBlock);
                        return IndexBlock.IndexBlock.pos;
                    }
                }



                IndexBlock.IndexBlock.pos = AllocateIndexBlock();
                    
                IndexBlock.IndexBlock.beginIndex = m_IndexFileHead.HeadBlock.DataEndPos;
                IndexBlock.IndexBlock.blockSize = size;
                IndexBlock.IndexBlock.Tag = 2;

                m_IndexFileHead.HeadBlock.DataEndPos += size;

                SyncToIndexFile(0, m_IndexFileHead);
                SyncToIndexFile(IndexBlock.IndexBlock.pos, IndexBlock);

                return IndexBlock.IndexBlock.pos;
            }

            void free(unsigned int handle)
            {
                TIndexFileBlock IndexBlock;
                unsigned long beginIndex = 0;
                unsigned long endIndex = 0;
                unsigned long pos;

                if (handle == 0)
                {
                    return;
                }
                
                if (!IsInit())
                {
                    return;
                }

                if (!ReadIndexBlockByPos(handle,&IndexBlock))
                {
                    return;
                }
 

                /*当前归还块是否需要合并 不合并则需要存储当前索引块*/ 
                /*往前合并 当前块的首 是否为其他块的尾*/
                if (m_EndIndexMap.find(IndexBlock.IndexBlock.beginIndex) != m_EndIndexMap.end())
                {
                    endIndex = IndexBlock.IndexBlock.beginIndex;

                    pos = m_EndIndexMap[IndexBlock.IndexBlock.beginIndex].IndexBlock.pos;
                    IndexBlock.IndexBlock.beginIndex = m_EndIndexMap[IndexBlock.IndexBlock.beginIndex].IndexBlock.beginIndex;
                    IndexBlock.IndexBlock.blockSize += m_EndIndexMap[IndexBlock.IndexBlock.beginIndex].IndexBlock.blockSize;

                    m_EndIndexMap.erase(endIndex);
                    m_BeginIndexMap.erase(IndexBlock.IndexBlock.beginIndex);

                    IndexBlock.IndexBlock.pos = pos;
                    FreeIndexBlock(IndexBlock);
                }

                /*往后合并*/
                beginIndex = IndexBlock.IndexBlock.beginIndex + IndexBlock.IndexBlock.blockSize;
                if (m_BeginIndexMap.find(beginIndex) != m_BeginIndexMap.end())
                {
                    IndexBlock.IndexBlock.blockSize += m_BeginIndexMap[beginIndex].IndexBlock.blockSize;

                    pos = m_BeginIndexMap[beginIndex].IndexBlock.pos;

                    m_BeginIndexMap.erase(beginIndex);
                    m_EndIndexMap.erase(IndexBlock.IndexBlock.beginIndex + IndexBlock.IndexBlock.blockSize);

                    IndexBlock.IndexBlock.pos = pos;
                    FreeIndexBlock(IndexBlock);

                }

                /*record*/
                //IndexBlock.IndexBlock.pos = AllocateIndexBlock();
                if ( IndexBlock.IndexBlock.beginIndex + IndexBlock.IndexBlock.blockSize == m_IndexFileHead.HeadBlock.DataEndPos)
                {
                    m_IndexFileHead.HeadBlock.DataEndPos = IndexBlock.IndexBlock.beginIndex;
                    FreeIndexBlock(IndexBlock);
                    SyncToIndexFile(0, m_IndexFileHead);
                }
                else
                {
                    IndexBlock.IndexBlock.Tag = 1;
                    SyncToIndexFile( IndexBlock.IndexBlock.pos, IndexBlock);
                    m_BeginIndexMap[IndexBlock.IndexBlock.beginIndex] = IndexBlock;
                    m_EndIndexMap[IndexBlock.IndexBlock.beginIndex + IndexBlock.IndexBlock.blockSize] = IndexBlock;
                }

                //delete indexBlock;
            }

        private:

            bool ReadIndexBlockByPos(unsigned int pos, PIndexFileBlock indexBlock)
            {
                if (m_IndexFile == NULL || !m_IndexFile->IsOpen())
                {
                    return false;
                }


                if (m_IndexFile->lseek(pos,SEEK_SET) < 0)
                {
                    return false;
                }

                if (m_IndexFile->read(indexBlock,sizeof(TIndexFileBlock), sizeof(TIndexFileBlock)) < 0)
                {
                    return false;
                }

                if (indexBlock->IndexBlock.Tag != 2)
                {
                    /*无效索引块*/
                    return false;
                }
                return true;
            }


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





            unsigned int AllocateIndexBlock()
            {
                unsigned int handle = 0;
                TIndexFileBlock tmp;
                if (m_IndexFile == NULL || !m_IndexFile->IsOpen())
                {
                    return false;
                }

                if (!m_FreeIndexBlockCache.empty())
                {
                    //memcpy(indexBlock, &(m_FreeIndexBlockCache.front()), sizeof(TIndexFileBlock));
                    handle = (m_FreeIndexBlockCache.front()).IndexBlock.pos;
                    m_FreeIndexBlockCache.pop_front();
                }
                else
                {
                    handle = m_IndexFileHead.HeadBlock.EndPos;
                    m_IndexFileHead.HeadBlock.EndPos += sizeof(TIndexFileBlock);
                    SyncToIndexFile(0,m_IndexFileHead);
                    tmp.IndexBlock.pos = handle;
                    tmp.IndexBlock.Tag = 0;
                    SyncToIndexFile(handle, tmp);
                }

               return handle;
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
                    indexBlock.IndexBlock.Tag = 0;
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

                        if (tmpIndexBlock.IndexBlock.Tag == 0)
                        {
                            m_FreeIndexBlockCache.push_back(tmpIndexBlock);
                        }
                        else if (tmpIndexBlock.IndexBlock.Tag == 1)
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

