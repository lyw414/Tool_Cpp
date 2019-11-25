#pragma once
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
namespace LYW_CODE
{
    #define NullType_Json 0x00
    #define JsonType_Json 0x01
    #define ArrayType_Json 0x02
    #define StringType_Json 0x03
    #define IntType_Json 0x04
    #define DoubleType_Json 0x05
    #define BoolType_Json 0x06

    class JsonBuffer
    {
    private:
        std::list < void* >  m_buffPool;
        void* nowBuff;
        int leftSize;
        int buffSize;

    public:
        JsonBuffer()
        {
            leftSize = 0;
            buffSize = 4096;
        }

        JsonBuffer(int buffSize)
        {
            leftSize = 0;
            this->buffSize = buffSize;
        }

        void free()
        {
            for (auto p : m_buffPool)
            {
                ::free(p);
            }
            m_buffPool.clear();
            leftSize = 0;
        }

        void * malloc(int size)
        {
            void * res ;
            if (leftSize >= size)
            {
                res = nowBuff;
                nowBuff = (char *)nowBuff + size;
                leftSize -= size;
            }
            else
            {

                if ( size < buffSize )
                {
                    res = ::malloc(buffSize);
                    m_buffPool.push_back(res);
                    nowBuff = (char *)res + size;
                    leftSize = buffSize - size;
                }
                else
                {
                    res = ::malloc(size);
                    m_buffPool.push_back(res);
                }
            }
            return res;
        }
    };

    class JsonStrMap
    {
    private:
        typedef struct _mapNode
        {
            char* key;
            unsigned int keyLen;
            void* data;
            struct _mapNode * next;
        }TmapNode,* PmapNode;
        JsonBuffer m_memManager;

        int arraySize = 0;
        PmapNode* hashArray;
        /*inline unsigned int APHash(const char* str, int len)*/
        inline unsigned int APHash(const char* key, int keyLen,const char* key1, int keyLen1 )
        {
            unsigned int hash = 0;
            int i;
            for (i = 0; i < keyLen; i++)
            {
                if ((i & 1) == 0)
                {
                    hash ^= ((hash << 7) ^ (*key++) ^ (hash >> 3));
                }
                else
                {
                    hash ^= (~((hash << 11) ^ (*key++) ^ (hash >> 5)));
                }
            }
            for (i = 0; i < keyLen1; i++)
            {
                if ((i & 1) == 0)
                {
                    hash ^= ((hash << 7) ^ (*key1++) ^ (hash >> 3));
                }
                else
                {
                    hash ^= (~((hash << 11) ^ (*key1++) ^ (hash >> 5)));
                }
            }

            return hash % arraySize;
        }

    public:
        JsonStrMap(int size)
        {
            arraySize = size;
            hashArray = (PmapNode* )::malloc(sizeof(void*) * arraySize);
            memset(hashArray, 0x00, sizeof(void*) * size);
        }
        JsonStrMap()
        {
            arraySize = 2048;
            hashArray = (PmapNode* )::malloc(sizeof(void*) * arraySize);
            memset(hashArray, 0x00, sizeof(void*) * arraySize);
        }

        void* find(const char* key, int keyLen, const char* key1, int keyLen1)
        {
            unsigned int index = APHash(key, keyLen, key1, keyLen1);
            PmapNode bucket = hashArray[index];
            int LenOfKey = keyLen + keyLen1;

            while (bucket != NULL )
            {
                /*compare key if find recopy data*/
                if (bucket->keyLen == (unsigned int)LenOfKey && memcmp(key,bucket->key,keyLen) == 0 && memcmp(key1,bucket->key + keyLen,keyLen1) == 0  )
                {
                    return bucket->data;
                }
                else
                {
                    bucket = bucket->next;
                }
            }
            return NULL;
        }


        void add(const char* key, int keyLen,const char* key1, int keyLen1, void* data)
        {
            unsigned int index = APHash(key, keyLen, key1, keyLen1);
            PmapNode node = (PmapNode)m_memManager.malloc(sizeof(TmapNode));
            node->keyLen = keyLen + keyLen1;
            node->key = (char *)m_memManager.malloc( node->keyLen);

            memcpy( node->key, key, keyLen);
            memcpy( node->key + keyLen, key1, keyLen1);
            PmapNode bucket = hashArray[index];
            if (bucket == NULL)
            {
                hashArray[index] = node;
                node->data = data;
                node->next = NULL;
                return;
            }
            while (bucket->next != NULL )
            {
                /*compare key if find recopy data*/
                if (bucket->keyLen == node->keyLen && memcmp(node->key,bucket->key,node->keyLen) == 0)
                {
                    bucket->data = data;
                    return;
                }
                bucket = bucket->next;
            }

            if (bucket->keyLen == node->keyLen && memcmp(node->key,bucket->key,node->keyLen) == 0)
            {
                bucket->data = data;
                return;
            }
            node->data = data;
            node->next = NULL;
            bucket->next = node;
        }
        void free()
        {
            m_memManager.free();
            memset(hashArray, 0x00, sizeof(void*) * arraySize);
        }

    };

    class Json
    {
    public:
        struct _JsonNode;
        unsigned int m_NodeID;

        typedef struct _ArrayObject
        {
            unsigned int id;
            unsigned int num;
        }TArrayObject;

        typedef struct _JsonObject
        {
            unsigned int id;
        }TJsonObject;

        typedef struct _JsonStr
        {
            char* str;
            unsigned int len;
        }TJsonStr;

        typedef union _Value
        {
            double m_double;
            int m_int;
            bool m_bool;
            TJsonStr m_str;
            TArrayObject m_arrayObject;
            TJsonObject m_jsonObject;
        }TValue;

        typedef struct _JsonNode
        {
            unsigned char m_type;
            TValue m_value;
        } TJsonNode, * PJsonNode;

        PJsonNode m_head;
        PJsonNode m_DeepNode;
        JsonBuffer m_memManager;
        /*std::unordered_map <std::string, TJsonNode* > m_Map;*/
        JsonStrMap m_Map;
        std::list < PJsonNode > m_Stack;

        inline void skip_space_and_enter(char** psStr, int& LenOfStr)
        {
            while (LenOfStr > 0)
            {
                switch (**psStr)
                {
                case ' ':
                case '\n':
                case '\r':
                    (*psStr)++;
                    LenOfStr--;
                    continue;
                default:
                    break;
                }
                break;
            }
        }

        /*skip num of char */
        inline void skip(char** psStr, int& LenOfStr, int num)
        {
            *psStr += num;
            LenOfStr -= num;
        }

        /*
 *          @brief  skip to end utill get strKey  end : must be '"' ':' not think aboat \
 *                   */
        bool getKey(char** psStr, int& LenOfStr, int& LenOfKey)
        {
            /*must be '\"'*/
            LenOfKey = 0;
            int backIndex = 1;
            while ( LenOfStr > 0 )
            {
                if ((*psStr)[0] != ':')
                {
                    LenOfStr--;
                    (*psStr)++;
                    LenOfKey++;
                }
                else
                {
                    /*backtrace untill '"' ,but only separator char can appear*/
                    while (1)
                    {
                        if (LenOfKey < backIndex)
                        {
                            throw std::runtime_error("Json 4:: JsonStr Format Error");
                        }
                        switch ((*psStr - backIndex)[0])
                        {
                        case ' ':
                        case '\r':
                            backIndex++;
                            continue;
                        case '"':
                            LenOfKey =  LenOfKey - backIndex;
                            LenOfStr--;
                            (*psStr)++;
                            return true;
                        default:
                            backIndex=1;
                            LenOfStr--;
                            (*psStr)++;
                            LenOfKey++;
                            break;
                        }
                        break;
                    }
                }
            }
            return false;
        }

        PJsonNode getValue(char** psStr, int& LenOfStr)
        {
            PJsonNode value = (PJsonNode)m_memManager.malloc(sizeof(TJsonNode));
            int backIndex = 1;
            char* begin = *psStr;
            while (LenOfStr > 0)
            {
                switch ((*psStr)[0])
                {
                case '\"':
                    /*string*/
                    value->m_value.m_str.len = 0;
                    value->m_type = StringType_Json;
                    skip(psStr, LenOfStr, 1);
                    while ( LenOfStr > 0 )
                    {
                        switch ( (*psStr)[0] )
                        {
                        case ',':
                        case '}':
                        case ']':
                            /*backtrace untill '"' ,but only separator char can appear*/
                            while (1)
                            {
                                if (value->m_value.m_str.len < (unsigned int)backIndex)
                                {
                                    return NULL;
                                }
                                switch ((*psStr - backIndex)[0])
                                {
                                case ' ':
                                case '\r':
                                case '\n':
                                    backIndex++;
                                    continue;
                                case '"':
                                    value->m_value.m_str.len -= backIndex;
                                    value->m_value.m_str.str = (char *)m_memManager.malloc(value->m_value.m_str.len);
                                    memcpy(value->m_value.m_str.str, begin + 1, value->m_value.m_str.len);
                                    return value;
                                default:
                                    backIndex=1;
                                    LenOfStr--;
                                    (*psStr)++;
                                    value->m_value.m_str.len++;
                                    break;
                                }
                                break;
                            }
                            break;
                        default:
                            LenOfStr--;
                            (*psStr)++;
                            value->m_value.m_str.len++;
                        }
                    }
                    return NULL;
                case '-':
                    skip(psStr, LenOfStr, 1);
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '.':
                    /*num int or double*/
                    value->m_type = IntType_Json;
                    while ( LenOfStr > 0 )
                    {
                        if ((*psStr)[0] >= '0' && (*psStr)[0] <= '9')
                        {
                            skip(psStr, LenOfStr, 1);
                        }
                        else if ((*psStr)[0] == '.')
                        {
                            /*double*/
                            if (value->m_type != IntType_Json)
                            {
                                return NULL;
                            }
                            else
                            {
                                value->m_type = DoubleType_Json;
                            }
                            skip(psStr, LenOfStr, 1);
                        }
                        else
                        {
                            while (LenOfStr > 0)
                            {
                                switch ((*psStr)[0])
                                {
                                case '\n':
                                case '\r':
                                case ' ':
                                    skip(psStr, LenOfStr, 1);
                                    break;
                                case ',':
                                case ']':
                                case '}':
                                    /*end flg*/
                                    if (value->m_type == IntType_Json)
                                    {
                                        value->m_value.m_int = ::atoi(begin);
                                    }
                                    else
                                    {
                                        value->m_value.m_double = ::atof(begin);
                                    }
                                    return value;
                                default :
                                    return NULL;
                                }
                            }
                            return NULL;
                        }
                    }
                    return NULL;
                case 'f':
                case 'F':
                    value->m_type = BoolType_Json;
                    if ( LenOfStr > 5 && (memcmp(*psStr,"false",5) == 0 || memcmp(*psStr,"FALSE",5 ) == 0 ) )
                    {
                        skip(psStr, LenOfStr, 5);
                        while (LenOfStr > 0)
                        {
                            switch ((*psStr)[0])
                            {
                            case ' ':
                            case '\n':
                            case '\r':
                                skip(psStr, LenOfStr, 1);
                                continue;
                            case ',':
                            case '}':
                            case ']':
                                value->m_value.m_bool = false;
                                return value;
                            default:
                                return NULL;
                            }
                        }
                    }
                    return NULL;
                case 't':
                case 'T':
                    value->m_type = BoolType_Json;
                    if ( LenOfStr > 4 && ( memcmp(*psStr,"TRUE",4) == 0 || memcmp(*psStr,"true",4) == 0))
                    {
                        skip(psStr, LenOfStr, 4);
                        while (LenOfStr > 0)
                        {
                            switch ((*psStr)[0])
                            {
                            case ' ':
                            case '\n':
                            case '\r':
                                skip(psStr, LenOfStr, 1);
                                continue;
                            case ',':
                            case '}':
                            case ']':
                                value->m_value.m_bool = true;
                                return value;
                            default:
                                return NULL;
                            }
                        }
                    }
                    return NULL;
                case 'n':
                case 'N':
                    value->m_type = NullType_Json;
                    if ( LenOfStr > 4 && ( memcmp(psStr,"NULL",4) == 0 || memcmp(psStr,"null",4) == 0 ) )
                    {
                        skip(psStr, LenOfStr,4);
                        while (LenOfStr > 0)
                        {
                            switch ((*psStr)[0])
                            {
                            case ' ':
                            case '\n':
                            case '\r':
                                skip(psStr, LenOfStr, 1);
                                continue;
                            case ',':
                            case '}':
                            case ']':
                                return value;
                            default:
                                return NULL;
                            }
                        }
                    }
                    return NULL;
                case '{':
                    skip(psStr, LenOfStr, 1);
                    value->m_type = JsonType_Json;
                    value->m_value.m_jsonObject.id = m_NodeID;
                    m_NodeID++;
                    return value;
                case '[':
                    skip(psStr, LenOfStr, 1);
                    value->m_type = ArrayType_Json;
                    value->m_value.m_arrayObject.id = m_NodeID;
                    value->m_value.m_arrayObject.num = 0;
                    m_NodeID++;
                    return value;
                default:
                    return NULL;
                }
            }
            return NULL;
        }

        bool endCheck(char** psStr, int& LenOfStr)
        {
            while (LenOfStr > 0)
            {
                switch ((*psStr)[0])
                {
                case ' ':
                case '\n':
                case '\r':
                    skip(psStr, LenOfStr, 1);
                    continue;
                default:
                    return false;
                }
            }
            return true;
        }


    public:
        Json()
        {
            m_head = NULL;
            m_NodeID = 0;
        }

        int parse(std::string& jsonStr)
        {
            return parse((char*)jsonStr.c_str(), jsonStr.length());
        }

        int parse(char* jsonStr, int len)
        {
            int LeftLen = len;
            /*int recodeLen = 0;*/
            char* keyBegin = NULL;
            int keyLen = 0;

            /*char* valueBegin = NULL;*/
            /*int valeuLen = 0;*/

            PJsonNode node;
            PJsonNode tmpnode;

            m_memManager.free();
            m_Stack.clear();
            m_Map.free();

            skip_space_and_enter(&jsonStr, LeftLen);
            switch (jsonStr[0])
            {
            case '{':
                node = m_head = (PJsonNode)m_memManager.malloc(sizeof(TJsonNode));
                m_head->m_type = JsonType_Json;
                m_head->m_value.m_jsonObject.id = m_NodeID;
                m_NodeID++;
                break;
            case '[':
                node = m_head = (PJsonNode)m_memManager.malloc(sizeof(TJsonNode));
                m_head->m_type = ArrayType_Json;
                m_head->m_value.m_arrayObject.id = m_NodeID;
                m_NodeID++;
                m_head->m_value.m_arrayObject.num = 0;
                break;
            default:
                return -1;
            }
            m_DeepNode = m_head;
            skip(&jsonStr, LeftLen, 1);
            skip_space_and_enter(&jsonStr, LeftLen);
            while (LeftLen > 0)
            {
                /*value analysis if father is jsonObject need analysis key-value or array*/
                switch (node->m_type)
                {
                case JsonType_Json:
                    skip_space_and_enter(&jsonStr, LeftLen);
                    /*end check */
                    if (jsonStr[0] == '}')
                    {
                        if (!m_Stack.empty())
                        {
                            /*pop*/
                            node = m_Stack.back();
                            m_Stack.pop_back();
                            skip(&jsonStr, LeftLen, 1);
                            if ( LeftLen < 0 )
                            {
                                throw std::runtime_error("json error:: jsonStr format error!");
                                return -1;
                            }
                            /* ',' check */
                            if (jsonStr[0] == ',')
                            {

                                skip(&jsonStr, LeftLen, 1);
                                if (LeftLen < 0 )
                                {
                                    throw std::runtime_error("json error:: jsonStr format error!");
                                    return -1;
                                }
                            }
                            break;
                        }
                        else
                        {
                            skip(&jsonStr, LeftLen, 1);
                            if (endCheck(&jsonStr, LeftLen))
                            {
                                return 1;
                            }
                            else
                            {
                                throw std::runtime_error("json error:: jsonStr format error!");
                                return -1;
                            }
                        }
                    }
                    while (true)
                    {
                        /*get all key - value untill new json or array*/
                        /*key*/
                        skip_space_and_enter(&jsonStr, LeftLen);
                        if (jsonStr[0] != '\"')
                        {
                            throw std::runtime_error("json error:: keyStr not Str!");
                            return -1;
                        }
                        skip(&jsonStr, LeftLen, 1);
                        keyBegin = jsonStr;
                        if (!getKey(&jsonStr, LeftLen, keyLen))
                        {
                            throw std::runtime_error("json error:: get keyStr failed!");
                            return -1;
                        }
                        /*value*/
                        skip_space_and_enter(&jsonStr, LeftLen);
                        tmpnode = getValue(&jsonStr, LeftLen);

                        /*add to map*/
                        /*m_Map[std::string((char*)&node->m_value.m_jsonObject.id, 4) + std::string(keyBegin, keyLen)] = tmpnode;*/
                        m_Map.add((char*)&node->m_value.m_jsonObject.id, 4, keyBegin, keyLen, tmpnode);

                        if (tmpnode->m_type == JsonType_Json || tmpnode->m_type == ArrayType_Json)
                        {
                            m_Stack.push_back(node);
                            node = tmpnode;
                            break;
                        }
                        /*end check*/
                        skip_space_and_enter(&jsonStr, LeftLen);
                        if (jsonStr[0] == ',')
                        {
                            /*skip this char*/
                            skip(&jsonStr, LeftLen, 1);
                            if ( LeftLen < 0 )
                            {
                                throw std::runtime_error("json error:: Len error!");
                                return -1;
                            }
                            continue;
                        }
                        else if (jsonStr[0] == '}')
                        {
                            break;
                        }
                        else
                        {
                            throw std::runtime_error("json error:: jsonStr Format Error! line [" + std::to_string(__LINE__) + "]");
                            return -1;
                        }
                    }
                    break;
                case ArrayType_Json:
                    /*end check */
                    if (jsonStr[0] == ']')
                    {
                        if (!m_Stack.empty())
                        {
                            /*pop*/
                            node = m_Stack.back();
                            m_Stack.pop_back();

                            skip(&jsonStr, LeftLen, 1);
                            if ( LeftLen < 0 )
                            {
                                throw std::runtime_error("json error:: jsonStr format error!");
                                return -1;
                            }
                            /* ',' check */
                            if (jsonStr[0] == ',')
                            {
                                skip(&jsonStr, LeftLen, 1);
                                if ( LeftLen < 0 )
                                {
                                    throw std::runtime_error("json error:: jsonStr format error!");
                                    return -1;
                                }
                            }
                            break;
                        }
                        else
                        {
                            skip(&jsonStr, LeftLen, 1);
                            if (endCheck(&jsonStr, LeftLen))
                            {
                                return 1;
                            }
                            else
                            {
                                throw std::runtime_error("json error:: jsonStr format error!");
                                return -1;
                            }
                        }
                    }
                    while (true)
                    {
                        /*value*/
                        tmpnode = getValue(&jsonStr, LeftLen);
                        /*m_Map[std::string((char*)&node->m_value.m_arrayObject.id, 4) + std::string((char*)&node->m_value.m_arrayObject.num, 4)] = tmpnode;*/
                        m_Map.add((char*)&node->m_value.m_arrayObject.id, 4,(char*)&node->m_value.m_arrayObject.num, 4, tmpnode);
                        node->m_value.m_arrayObject.num++;
                        if (tmpnode->m_type == JsonType_Json || tmpnode->m_type == ArrayType_Json)
                        {
                            m_Stack.push_back(node);
                            node = tmpnode;
                            break;
                        }
                        /*end check*/
                        skip_space_and_enter(&jsonStr, LeftLen);
                        if (jsonStr[0] == ',')
                        {
                            /*skip this char*/
                            skip(&jsonStr, LeftLen, 1);
                            if (LeftLen >= 0 )
                            {
                                continue;
                            }
                            else
                            {
                                throw std::runtime_error("json error:: Len error!");
                                return -1;
                            }
                        }
                        else if (jsonStr[0] == ']')
                        {
                            break;
                        }
                        else
                        {
                            throw std::runtime_error("json error:: jsonStr Format Error! line [" + std::to_string(__LINE__) + "]");
                            return -1;
                        }
                    }
                    break;
                default:
                    return -1;
                }
            }
            return 0;
        }

        Json& operator [] (char * key)
        {
            Json* res = NULL;
            if (m_DeepNode->m_type != JsonType_Json)
            {
                m_DeepNode = m_head;
                return *res;
            }
            else
            {
                PJsonNode tmp = (PJsonNode)m_Map.find((char*)&m_DeepNode->m_value.m_jsonObject.id, 4, key, strlen(key));
                if (tmp == NULL)
                {
                    tmp = (PJsonNode)m_memManager.malloc(sizeof(TJsonNode));
                    m_Map.add((char*)&m_DeepNode->m_value.m_jsonObject.id, 4, key, strlen(key), tmp);
                    tmp->m_type = NullType_Json;
                    m_DeepNode = tmp;
                    return *this;
                }
                else
                {
                    m_DeepNode = tmp;
                    return *this;
                }
            }

        }
        Json & operator [] (std::string& key)
        {
            Json* res = NULL;
            if (m_DeepNode->m_type != JsonType_Json)
            {
                m_DeepNode = m_head;
                return *res;
            }
            else
            {
                PJsonNode tmp = (PJsonNode)m_Map.find((char*)&m_DeepNode->m_value.m_jsonObject.id, 4, key.c_str(), key.length());
                if ( tmp == NULL)
                {
                    PJsonNode tmp = (PJsonNode)m_memManager.malloc(sizeof(TJsonNode));
                    m_Map.add((char*)&m_DeepNode->m_value.m_jsonObject.id, 4,key.c_str(),key.length(),tmp);
                    tmp->m_type = NullType_Json;
                    m_DeepNode = tmp;
                    return *this;
                }
                else
                {
                    return *this;
                }
            }
        }

        Json & operator [] (int index)
        {
            Json* res = NULL;
            if (m_DeepNode->m_type != ArrayType_Json)
            {
                m_DeepNode = m_head;
                return *res;
            }
            else
            {
                if (index == -1 || m_DeepNode->m_value.m_arrayObject.num <= index )
                {
                    PJsonNode tmp = (PJsonNode)m_memManager.malloc(sizeof(TJsonNode));
                    m_Map.add((char*)&m_DeepNode->m_value.m_arrayObject.id, 4, (char*)&m_DeepNode->m_value.m_arrayObject.num, 4,tmp);
                    tmp->m_type = NullType_Json;
                    m_DeepNode->m_value.m_arrayObject.num++;
                    m_DeepNode = tmp;
                    return *this;
                }
                m_DeepNode = (PJsonNode)m_Map.find((char*)&m_DeepNode->m_value.m_arrayObject.id, 4, (char *)&index,4);
                return *this;
            }
        }

        operator int()
        {
            int Int = 0;
            if (m_DeepNode != NULL)
            {
                Int = m_DeepNode->m_value.m_int;
                m_DeepNode = m_head;
            }
            return Int;
        }

        Json & operator = (int Int)
        {
            m_DeepNode->m_type = IntType_Json;
            m_DeepNode->m_value.m_int = Int;
            m_DeepNode = m_head;
            return *this;
        }



        operator double()
        {
            double data = 0;
            if (m_DeepNode != NULL)
            {
                data = m_DeepNode->m_value.m_double;
                m_DeepNode = m_head;
            }
            return data;
        }

        Json & operator = (double data)
        {
            m_DeepNode->m_type = DoubleType_Json;
            m_DeepNode->m_value.m_double = data;
            m_DeepNode = m_head;
            return *this;
        }


        operator float()
        {
            double data = 0;
            if (m_DeepNode != NULL)
            {
                data = m_DeepNode->m_value.m_double;
                m_DeepNode = m_head;
            }
            return data;
        }

        Json & operator = (float data)
        {
            m_DeepNode->m_type = DoubleType_Json;
            m_DeepNode->m_value.m_double = data;
            m_DeepNode = m_head;
            return *this;
        }

        operator bool()
        {
            bool data = 0;
            if (m_DeepNode != NULL)
            {
                data = m_DeepNode->m_value.m_bool;
                m_DeepNode = m_head;
            }
            return data;
        }

        Json & operator = (bool data)
        {
            m_DeepNode->m_type = BoolType_Json;
            m_DeepNode->m_value.m_bool= data;
            m_DeepNode = m_head;
            return *this;
        }

        operator std::string()
        {
            std::string data = "";
            if (m_DeepNode != NULL)
            {
                data = std::string(m_DeepNode->m_value.m_str.str, m_DeepNode->m_value.m_str.len);
                m_DeepNode = m_head;
            }
            return data;
        }

        Json & operator = (std::string data)
        {
            m_DeepNode->m_type = StringType_Json;
            m_DeepNode->m_value.m_str.str = (char *)m_memManager.malloc(data.length());
            ::memcpy(m_DeepNode->m_value.m_str.str, data.c_str(), data.length());
            m_DeepNode->m_value.m_str.len = data.length();
            m_DeepNode = m_head;
            return *this;
        }
    };
}
