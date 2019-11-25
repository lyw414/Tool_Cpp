#pragma once
#include <vector>
#include <list>
namespace LYW_CODE
{
	struct _JsonValue;
	typedef struct _StrMapNode_Json
	{
		char * sKey;
		short len;
		struct _JsonValue * m_data;
	}TStrMapNode_Json, * PStrMapNode_Json;

	class StrMap_Json
	{
	private:
		int m_size;
		StrMap_Json(int size) : m_Bucket(size)
		{
			m_size = size;
		}
		std::vector	< std::list < PStrMapNode_Json > > m_Bucket;
	public:
		bool add(const char * sKey, short iLenOfKey, struct _JsonValue * data)
		{
			return true;
		}
		struct _JsonValue * find ( const char * sKey, short iLenOfKey, struct _JsonValue * data )
		{
			return NULL;
		}
		bool find_add ( const char * sKey, short iLenOfKey, void ** data)
		{
			return true;
		}
		bool del()
		{
			return true;
		}
		std::vector	< std::list < PStrMapNode_Json > > & begin()
		{
			return m_Bucket;
		}
	};
}
