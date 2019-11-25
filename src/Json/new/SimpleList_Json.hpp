#pragma once
#pragma pack(16)
#include <stdlib.h>
#include <string.h>
namespace LYW_CODE
{
	struct _JsonValue;
	typedef struct _SimpleListNode_Json
	{
		char * sKey;
		short len;
		struct _JsonValue * data;
		struct _SimpleListNode_Json * next;
	}TSimpleListNode_Json, * PSimpleListNode_Json;
	class SimpleList_Json
	{
	private:
		PSimpleListNode_Json m_begin;
	public:
		SimpleList_Json()
		{
			m_begin = NULL;
		}
        ~SimpleList_Json()
        {
            clear();
        }
		bool add ( char * sKey, short iLenOfKey, struct _JsonValue * data)
		{
			return true;
		}
		struct _JsonValue * find ( const char * sKey, short iLenOfKey, struct _JsonValue * data )
		{
			return NULL;
		}
		/*
		 * @brief search node in SimpleList , if find it data point to the value of m_data, 
		 * else add a new node key witch key is sKey data point to address of node's m_data use to copy pointor to it
		 * return not found false 
		              found true
		 */
		bool find_add ( char * sKey, short iLenOfKey, void ** data)
		{
			PSimpleListNode_Json node = m_begin;
			while (node != NULL )
			{
				if (node->len == iLenOfKey && memcmp(node->sKey, sKey, iLenOfKey) == 0)
				{
					*data = node->data;
					return true;
				}
				node = node->next;
			}
			//empty or not found, add new node 
			node = (PSimpleListNode_Json)malloc(sizeof(TSimpleListNode_Json));
			node->sKey = sKey;
			node->len = iLenOfKey;
			node->next = m_begin;
			*data = &node->data;
			//add to head 
			m_begin = node;
			return false;
		}

		bool find_add_copy(char * sKey, short iLenOfKey, void ** data)
		{
			PSimpleListNode_Json node = m_begin;
			while (node != NULL )
			{
				if (node->len == iLenOfKey && memcmp(node->sKey, sKey, iLenOfKey) == 0)
				{
					*data = node->data;
					return true;
				}
				node = node->next;
			}
			//empty or not found, add new node 
			node = (PSimpleListNode_Json)malloc(sizeof(TSimpleListNode_Json));
			node->len = iLenOfKey;
			node->sKey = (char *)malloc(node->len);
			memcpy(node->sKey, sKey, iLenOfKey);
			node->next = m_begin;
			*data = &node->data;
			//add to head 
			m_begin = node;
			return false;
		}
		bool del()
		{
			return true;
		}
		void clear()
		{
			PSimpleListNode_Json node;
			while ( m_begin != NULL)
			{
				node = m_begin;
				m_begin = m_begin->next;
				::free(node);
			}
		}

		inline PSimpleListNode_Json & begin()
		{
			return m_begin;
		}

	};

}
