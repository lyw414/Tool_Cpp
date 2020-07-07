#include "FileStorage.hpp"
#define B_TREE_RANK 4u

namespace LYW_CODE
{
    typedef struct _B_TreeKey
    {
        unsigned int keyLen;
        unsigned char keyData[0];
    }TB_TreeKey;

    typedef struct _B_TreeData
    {
        unsigned int dataLen;
        unsigned char Data[0];
    }TB_TreeData;


    typedef struct _B_TreeNode
    {
        /*子树根节点*/
        DataHanlde childNode[B_TREE_RANK];
        /*节点数据单元*/
        DataHanlde key[B_TREE_RANK - 1];
        DataHanlde data[B_TREE_RANK - 1];
    }TB_TreeNode;

    typedef TB_TreeNode THeadNode;
    
    class B_Tree
    {
        public:
            typedef int (*TCmpFunc)(TB_TreeKey * key, TB_TreeKey * key2);

            /*添加根节点的文件存储句柄*/
            B_Tree( TCmpFunc cmpFunc, const DataHanlde & rootNode) : m_fileStorage()
            {
                memset (&m_ZeroNodeBuf, 0x00, sizeof( TB_TreeNode ));
                m_rootNode = rootNode;
                if (cmpFunc != NULL)
                {
                    m_cmpFunc = cmpFunc;
                }
            }
            
            /*allocate root node*/
            B_Tree(TCmpFunc cmpFunc) : m_fileStorage()
            {
                memset (&m_ZeroNodeBuf, 0x00, sizeof( TB_TreeNode ));
                NewNode(&m_rootNode);
                if (cmpFunc != NULL)
                {
                    m_cmpFunc = cmpFunc;
                }
            }

            /**/
            void Insert(TB_TreeKey * key, TB_TreeData * data)
            {
                key = NULL;
                data = NULL;

                //bool stopFlag = false;
                TB_TreeNode nodeData;
                TB_TreeKey nodeKey;

                m_fileStorage.LoadData(m_rootNode, &nodeData, sizeof(TB_TreeNode));

                while(1)
                {

                    for(unsigned int iLoop = 0; iLoop < B_TREE_RANK - 1; iLoop++  )
                    {
                        if (m_fileStorage.IsNULL(nodeData.key[iLoop]))
                        {
                            /*not found*/
                            break;
                        }
                        else
                        {
                            /*read keyData*/
                            m_fileStorage.LoadData( nodeData.key[iLoop], &nodeKey, sizeof(TB_TreeKey));
                            if (m_cmpFunc(key, &nodeKey) == 0)
                            {
                                /*equal , update Data*/
                                //m_fileStorage.ffree(nodeData.data[iLoop], , );

                            }
                            else if (m_cmpFunc(key, &nodeKey) == 1)
                            {
                                /*less than, sink down*/
                            }
                            else
                            {
                                continue;
                            }

                        }

                    }

                    /*not found*/
                    break;
                }


            }


            /*find*/
            int Find(TB_TreeKey * key, TB_TreeData * data)
            {
                if (key == NULL)
                {
                    return 1;
                }

                data = NULL;
                return 1;
            }


        private:

            TB_TreeNode m_ZeroNodeBuf;
            TCmpFunc m_cmpFunc;

            bool NewNode(DataHanlde * dataHandle )
            {
                *dataHandle = m_fileStorage.falloc(sizeof(TB_TreeNode));
                m_fileStorage.StorageData(*dataHandle, &m_ZeroNodeBuf, sizeof(TB_TreeNode));
                return true;
            }

            DataHanlde m_rootNode;

            FileStorage m_fileStorage;
    };

}

