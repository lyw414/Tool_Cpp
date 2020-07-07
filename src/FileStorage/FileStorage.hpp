#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>
#include <mutex>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#define FILE_STORAGE_BLOCK_SIZE 4096 /*allocate buffer*/
#define FHANDLE LYW_CODE::FileBlockInfo
namespace LYW_CODE
{
    typedef struct _FileBlockInfo
    {
        unsigned long beginIndex;
        unsigned long blockSize;
        unsigned long FileIndex;
        unsigned long UseFlag;
    } FileBlockInfo;

    typedef struct _FixedHeadIndexFile
    {
        unsigned long BlockNum;
        unsigned long EndIndex;
    } FixedHeadIndexFile;



    class FileStorage
    {
    public:
        FileStorage()
        {
            m_FileName = "default.data";
            m_IndexFileName = "default_index.data";

            m_FileHandle = -1;
            m_IndexFileHandle = -1;
            m_IndexMap_Begin.clear();
            m_IndexMap_End.clear();

            memset(&m_FixedHead, 0x00, sizeof(FixedHeadIndexFile));
        }

        FileStorage(const std::string &FileName)
        {
            m_FileName = FileName + ".data";
            m_IndexFileName = FileName + "_index.data";

            m_FileHandle = 1;
        }

        ~FileStorage()
        {
            SyncToIndexFile();
        }

        FileBlockInfo falloc(size_t size)
        {
            FileBlockInfo blockInfo;
            memset(&blockInfo, 0x00, sizeof(FileBlockInfo));
            if (!IsFileOpen())
            {
                return blockInfo;
            }

            for (auto &p : m_IndexMap_Begin)
            {
                if (p.second.blockSize >= size)
                blockInfo = p.second;
                break;
            }

            if (blockInfo.blockSize > 0)
            {
                m_IndexMap_Begin.erase(blockInfo.beginIndex);
                m_IndexMap_End.erase(blockInfo.beginIndex + blockInfo.blockSize);
                return blockInfo;
            }

            blockInfo.beginIndex = m_FixedHead.EndIndex;
            blockInfo.blockSize = size;
            m_FixedHead.EndIndex += size;
            //m_FixedHead.BlockNum++;

            return blockInfo;
        }

        void ffree (const FileBlockInfo &Handle)
        {
            FreeBlock(&Handle);
        }
        
        /**
         * @brief Get Data From File
         *
         * @param Handle File Block Info
         * @param Data Data
         *
         * @return -1
         */
        int LoadData(const FileBlockInfo &Handle,void * Data, size_t iSizeOfData)
        {
            int ret = -1;
            if (!IsFileOpen())
            {
                return -1;
            }

            if (Handle.blockSize > iSizeOfData)
            {
                return -1;
            }

            ::lseek(m_FileHandle, Handle.beginIndex, SEEK_SET);
            ret = ::read(m_FileHandle, Data, Handle.blockSize);
            if (ret <= 0 || ret != (int)Handle.blockSize)
            {
                return -1; 
            }
            return ret;
        }



        //void StorageData(const FileBlockInfo &Handle, void *Data)
        //{
        //}
        

        /**
         * @brief Read Data From File
         *
         * @param Handle
         * @param Data
         * @param iLenOfData
         */
        int StorageData(const FileBlockInfo &Handle, void *Data, size_t iLenOfData)
        {
            int ret = -1;
            if (!IsFileOpen())
            {
                return -1;
            }

            if (Handle.blockSize < iLenOfData)
            {
                return -1;
            }

             ::lseek(m_FileHandle, Handle.beginIndex, SEEK_SET);
            ret = ::write(m_FileHandle, Data, iLenOfData);
            return 0;
        }
    private:

        bool IsFileOpen()
        {
            if (m_FileHandle < 0)
            {
                /*File Not Open, open it*/
                if ((m_FileHandle = open(m_FileName.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0)
                {
                    return false;
                }
            }

            if (m_IndexFileHandle < 0)
            {
                if ((m_IndexFileHandle = open(m_IndexFileName.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0)
                {
                    return false;
                }
                else
                {
                    if (LoadIndexFile() < 0 )
                    {
                        return false;
                    }
                }


            }
            return true;
        }

        

        /**
         * @brief  Load Block Info To Map
         * 
         *
         * @return -1 file not open
         *        >=0 Num of Free Block
         *
         */
        int LoadIndexFile()
        {
            int ret = -1;
            FileBlockInfo blockInfo;

            if (m_IndexFileHandle < 0)
            {
                return -1;
            }

            ret = ::read(m_IndexFileHandle, &m_FixedHead, sizeof(FixedHeadIndexFile));
            if (ret != sizeof(FixedHeadIndexFile))
            {
                /*Head Error Reset File*/
                m_FixedHead.BlockNum = 0;
                m_FixedHead.EndIndex = 0;
                m_IndexMap_Begin.clear();
                m_IndexMap_End.clear();
            }
            else
            {
                /*Load Block Info to Map*/
                unsigned long iLoop = m_FixedHead.BlockNum;
                printf("Fixed Head [%ld] [%ld]\n", m_FixedHead.BlockNum, m_FixedHead.EndIndex);

                m_FixedHead.BlockNum = 0;
                for(; iLoop> 0; iLoop--)
                {
                    ret = ::read(m_IndexFileHandle, &blockInfo, sizeof(FileBlockInfo));
                    if (ret != sizeof(FileBlockInfo))
                    {
                        m_FixedHead.EndIndex = 0;
                        m_IndexMap_Begin.clear();
                        m_IndexMap_End.clear();
                        return 0;
 
                    }
                    FreeBlock(&blockInfo);
                }
            }
            return ret;
        }
    public:
        void SyncToIndexFile()
        {
            int ret = 0;
            if (!IsFileOpen())
            {
                return;
            }

            if (m_FixedHead.BlockNum != m_IndexMap_Begin.size())
            {
                m_FixedHead.BlockNum = m_IndexMap_Begin.size();
            }
            ::lseek(m_IndexFileHandle, 0, SEEK_SET);
            ret = ::write(m_IndexFileHandle, &m_FixedHead, sizeof(FixedHeadIndexFile));
            for (auto &p : m_IndexMap_Begin)
            {
                ret = ::write(m_IndexFileHandle, &p.second,sizeof(FileBlockInfo));
            }
            
        }
    private:
        void FreeBlock(const FileBlockInfo *pBlockInfo)
        {
            unsigned long beginIndex = 0;

            if (!IsFileOpen())
            {
                return;
            }
            if (pBlockInfo == NULL)
            {

                return;
            }

            FileBlockInfo blockInfo;
            memcpy(&blockInfo, pBlockInfo, sizeof(FileBlockInfo));
            printf("Info[%ld] [%ld] ", blockInfo.beginIndex, blockInfo.blockSize);

            
            /*front merger*/
            beginIndex = blockInfo.beginIndex - blockInfo.blockSize;
            if ( blockInfo.beginIndex != 0 && m_IndexMap_End.find(beginIndex) != m_IndexMap_End.end())
            {
                blockInfo.blockSize += m_IndexMap_End[beginIndex].blockSize;
                blockInfo.beginIndex = m_IndexMap_End[beginIndex].beginIndex;

                m_FixedHead.BlockNum--;
                m_IndexMap_Begin.erase(blockInfo.beginIndex);
                m_IndexMap_End.erase( beginIndex);

                printf("front merger[%ld] [%ld] ", blockInfo.beginIndex, blockInfo.blockSize);
            }

            /*backward merger*/
            beginIndex = blockInfo.beginIndex + blockInfo.blockSize;
            if (m_IndexMap_Begin.find(beginIndex) != m_IndexMap_Begin.end())
            {
                blockInfo.blockSize += m_IndexMap_Begin[beginIndex].blockSize;
                m_FixedHead.BlockNum--;
                m_IndexMap_Begin.erase(beginIndex);
                m_IndexMap_End.erase(blockInfo.beginIndex + blockInfo.blockSize);

                printf("backward merger[%ld] [%ld] ", blockInfo.beginIndex, blockInfo.blockSize);
            }

            /*endCheck if beginIndex Is EndIndex No Need record*/
            beginIndex = blockInfo.beginIndex + blockInfo.blockSize;
            if (beginIndex >= m_FixedHead.EndIndex)
            {
                m_FixedHead.EndIndex = blockInfo.beginIndex;
                printf("Delete Last[%ld] [%ld]\n", blockInfo.beginIndex, blockInfo.blockSize);
            }
            else
            {
                m_IndexMap_Begin[blockInfo.beginIndex] = blockInfo;
                m_IndexMap_End[blockInfo.beginIndex + blockInfo.blockSize] = blockInfo;
                m_FixedHead.BlockNum++;
                printf("Add[%ld] [%ld]\n", blockInfo.beginIndex, blockInfo.blockSize);
            }

        }


        int m_FileHandle;
        int m_IndexFileHandle;
        FixedHeadIndexFile m_FixedHead;
        std::mutex m_Lock;
        std::string m_FileName;
        std::string m_IndexFileName;
        std::unordered_map <unsigned long, FileBlockInfo> m_IndexMap_Begin;
        std::unordered_map <unsigned long, FileBlockInfo> m_IndexMap_End;
    };
}
