//#include "src/FileStorage/FileStorage.hpp"
#include "FileStorage.hpp"
#include <string>
#include <vector>
namespace LYW_CODE
{

    class FileStrHashMap
    {
    private:
        /*文件存储结构*/
        typedef struct _MapNode_FS
        {
            FileStorageHandle keyData;
            FileStorageHandle Data;
            FileStorageHandle nextNode;
        }TMapNode_FS;

        typedef struct _Data_MM
        {
            unsigned int dataLen;
            unsigned char data[0];
        }TData_MM;

        typedef struct _MapNode_MM
        {
            TData_MM key;
            TData_MM data;
            FileStorageHandle nextNode;
        }TMapNode_MM;

        typedef struct _MapInfo
        {
            FileStorageHandle m_bucket;
            unsigned int m_maxSize;
        }TMapInfo;

        inline unsigned int APHash ( const char *str, int len )
        {
            unsigned int hash = 0;
            int i;
            for (i=0; i < len; i++)
            {
                if ((i & 1) == 0)
                {
                    hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
                }
                else
                {
                    hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
                }
            }
            return hash % m_info.m_maxSize;
        }

    public:

        FileStrHashMap() : m_storage("HashMapFile")
        {
            m_info.m_maxSize = 10240;
            m_info.m_bucket = 0;
        }

        FileStorageHandle Init()
        {
            S_info = m_storage.allocate(sizeof(TMapInfo));
            m_info.m_maxSize = 10240;
            printf("%d\n", S_info);
            m_info.m_bucket = m_storage.allocate(sizeof(FileStorageHandle) * m_info.m_maxSize);

            m_storage.write(S_info, &m_info, sizeof(TMapInfo));

            m_storage.fset(m_info.m_bucket,0x00);
            
            /*缓存桶*/ 
            m_bucket_cache = (FileStorageHandle *)malloc(sizeof(FileStorageHandle) * m_info.m_maxSize);
            m_storage.read(m_info.m_bucket, m_bucket_cache, sizeof(TMapNode_FS) * m_info.m_maxSize);
            return S_info;

        }
     
        FileStorageHandle Init(FileStorageHandle infoHandle)
        {
            S_info = infoHandle;
            if (m_storage.read(S_info, &m_info, sizeof(TMapInfo)) != sizeof(TMapInfo))
            {
                Init();
            }
            else
            {
                m_bucket_cache = (FileStorageHandle *)malloc(sizeof(FileStorageHandle) * m_info.m_maxSize);
                m_storage.read(m_info.m_bucket, m_bucket_cache, sizeof(TMapNode_FS) * m_info.m_maxSize);
            }

            return S_info;
        }


    unsigned int find(const std::string & key, void * data,unsigned int sizeOfData)
    {
        unsigned int index = APHash(key.c_str(), key.length());
        unsigned char *tmpBuf = NULL;
        unsigned int len = 0;
        unsigned int tmp = 0;
        FileStorageHandle nodeHandle;
        TMapNode_FS tmpNode;
        TData_MM *DataNode;
        if (m_bucket_cache[index] == 0)
        {
            return 0;
        }

        nodeHandle = m_bucket_cache[index];


        while (nodeHandle != 0)
        {
            m_storage.read(nodeHandle, &tmpNode, sizeof(TMapNode_FS));
            tmp =  m_storage.size(tmpNode.keyData);
            if (len < tmp)
            {
                len = tmp;
                if (tmpBuf != NULL)
                {
                    free(tmpBuf);
                }
                tmpBuf = (unsigned char *)malloc(len);
            }

            m_storage.read(tmpNode.keyData, tmpBuf, len);
            DataNode = (TData_MM *)tmpBuf;

            if (key == std::string((char *)DataNode->data,DataNode->dataLen))
            {
                tmp = m_storage.size(tmpNode.Data);
                if (len < tmp)
                {
                    len = tmp;
                    if (tmpBuf != NULL)
                    {
                        free(tmpBuf);
                    }
                    tmpBuf = (unsigned char *)malloc(len);
                }

                m_storage.read(tmpNode.Data, tmpBuf, len);
                DataNode = (TData_MM *)tmpBuf;
                if (DataNode->dataLen <= sizeOfData)
                {   
                    memcpy(data, DataNode->data, DataNode->dataLen);
                    return DataNode->dataLen;
                }
                else
                {
                    memcpy(data, DataNode->data, sizeOfData);
                    return sizeOfData;
                }

            }

            nodeHandle = tmpNode.nextNode;
        }

        return 0; 
    }

    bool add (const std::string & key, void * data,unsigned int lenOfData)
    {
        unsigned int index = APHash(key.c_str(), key.length());
        unsigned char *tmpBuf = NULL;
        unsigned int len = 0;
        unsigned int tmp = 0;
        FileStorageHandle nodeHandle;
        FileStorageHandle PreHandle;
        TMapNode_FS tmpNode;
        TData_MM *DataNode;

        nodeHandle = m_bucket_cache[index];
        while (nodeHandle != 0)
        {
            PreHandle = nodeHandle;
            m_storage.read(nodeHandle, &tmpNode, sizeof(TMapNode_FS));
            tmp =  m_storage.size(tmpNode.keyData);
            if (len < tmp)
            {
                len = tmp;
                if (tmpBuf != NULL)
                {
                    free(tmpBuf);
                }
                tmpBuf = (unsigned char *)malloc(len);
            }

            m_storage.read(tmpNode.keyData, tmpBuf, len);
            DataNode = (TData_MM *)tmpBuf;

            if (key == std::string((char *)DataNode->data,DataNode->dataLen))
            {

                tmp = lenOfData + sizeof(unsigned int);

                if (len < tmp)
                {
                    len = tmp;
                    if (tmpBuf != NULL)
                    {
                        free(tmpBuf);
                    }
                    tmpBuf = (unsigned char *)malloc(len);
                }


                DataNode = (TData_MM *)tmpBuf;

                m_storage.Free(tmpNode.Data);

                tmpNode.Data = m_storage.allocate(tmp);
                DataNode->dataLen = lenOfData;
                memcpy(DataNode->data, data, lenOfData);

                m_storage.write(nodeHandle,&tmpNode, sizeof(TMapNode_FS));
                m_storage.write(tmpNode.Data,DataNode,tmp);

                delete tmpBuf;
                len = 0;
                return true;
            }

            nodeHandle = tmpNode.nextNode;
        }

        nodeHandle = m_storage.allocate(sizeof(TMapNode_FS));
        
        TMapNode_FS node;

        node.nextNode = 0;

        tmp = key.length() + sizeof(unsigned int);
        node.keyData = m_storage.allocate(tmp);
        if (len < tmp)
        {
            len = tmp;
            if (tmpBuf != NULL)
            {
                free(tmpBuf);
            }
            tmpBuf = (unsigned char *)malloc(len);
        }

        DataNode = (TData_MM *)tmpBuf;
        DataNode->dataLen = key.length();
        memcpy(DataNode->data, key.c_str(), key.length());
        m_storage.write(node.keyData, DataNode, tmp);


        tmp = lenOfData + sizeof(unsigned int);
        node.Data = m_storage.allocate(tmp);
        if (len < tmp)
        {
            len = tmp;
            if (tmpBuf != NULL)
            {
                free(tmpBuf);
            }
            tmpBuf = (unsigned char *)malloc(len);
        }

        DataNode = (TData_MM *)tmpBuf;
        DataNode->dataLen = lenOfData;
        memcpy(DataNode->data, data, lenOfData);
        m_storage.write(node.Data, DataNode, tmp);

        delete tmpBuf;

        
        m_storage.write(nodeHandle, &node, sizeof(TMapNode_FS));

        if (m_bucket_cache[index] == 0)
        {
            m_bucket_cache[index] = nodeHandle;
            m_storage.write(m_info.m_bucket, m_bucket_cache, sizeof(FileStorageHandle) * m_info.m_maxSize);
        }
        else
        {
            tmpNode.nextNode = nodeHandle;
            m_storage.write(PreHandle,&tmpNode,sizeof(TMapNode_FS));
        }        
        return true;
    }


    bool del(const std::string & key )
    {
        unsigned int index = APHash(key.c_str(), key.length());
        unsigned char *tmpBuf = NULL;
        unsigned int len = 0;
        unsigned int tmp = 0;
        FileStorageHandle nodeHandle;
        FileStorageHandle preNode;
        TMapNode_FS tmpNode;
        TData_MM *DataNode;
        if (m_bucket_cache[index] == 0)
        {
            return 0;
        }

        nodeHandle = m_bucket_cache[index];
        preNode = 0;


        while (nodeHandle != 0)
        {
            m_storage.read(nodeHandle, &tmpNode, sizeof(TMapNode_FS));
            tmp =  m_storage.size(tmpNode.keyData);
            if (len < tmp)
            {
                len = tmp;
                if (tmpBuf != NULL)
                {
                    free(tmpBuf);
                }
                tmpBuf = (unsigned char *)malloc(len);
            }

            m_storage.read(tmpNode.keyData, tmpBuf, len);
            DataNode = (TData_MM *)tmpBuf;

            if (key == std::string((char *)DataNode->data,DataNode->dataLen))
            {

                m_storage.Free(tmpNode.keyData);
                m_storage.Free(tmpNode.Data);
                m_storage.Free(nodeHandle);
                nodeHandle = tmpNode.nextNode;
                if (preNode != 0)
                {
                    m_storage.read(preNode, &tmpNode, sizeof(TMapNode_FS));
                    tmpNode.nextNode = nodeHandle;
                    m_storage.write(preNode, &tmpNode, sizeof(TMapNode_FS));
                } 
                else
                {
                    m_bucket_cache[index] = nodeHandle;
                    m_storage.write(m_info.m_bucket, m_bucket_cache, sizeof(FileStorageHandle) * m_info.m_maxSize);
                }
            }

            preNode = nodeHandle;
            nodeHandle = tmpNode.nextNode;
        }

        return 0; 
    }


    private:
        LYW_CODE::FileStorage<> m_storage;
        /*内存信息 会在存储的mapinfo中为主*/
        TMapInfo m_info;
        std::string m_HeadFileName;
        int m_fileHandle;

         FileStorageHandle * m_bucket_cache;

    private :
        /*存储*/
        FileStorageHandle S_info;
    };

}
