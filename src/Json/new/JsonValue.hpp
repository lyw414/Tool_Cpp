#pragma once
#include "SimpleList_Json.hpp"
#include "StrMap_Json.hpp"
#include <vector>
#define TJsonValueType unsigned char 
/*
 * @brief _T means use JsonBuffer value
*/
#define NewType_Json 0x01
#define IntType_Json 0x02
#define DoubleType_Json 0x03
#define StringType_Json 0x04
#define BoolType_Json 0x05
#define NullType_Json 0x06
#define JsonType_Json 0x07
#define ArrayType_Json 0x08

namespace LYW_CODE
{
	typedef struct _String_Json
	{
		char * m_str;
		int m_len;
	}TString_Json, * PString_Json;

	typedef struct _JsonObject_Json
	{
		SimpleList_Json m_ChildNode;
	}TJsonObject_Json, * PJsonObject_Json;
	
	typedef struct _ArrayObject_Json
	{
		std::vector < struct _JsonValue * > m_ArrayNode;
	}TArrayObject_Json, * PArrayObject_Json;

	typedef struct _JsonValue
	{
		TJsonValueType m_type;
        union
        {
		long m_int;
		double m_double;
		bool m_bool;
		PJsonObject_Json m_json;
		PArrayObject_Json m_array;
		PString_Json m_string;
        };
	}TJsonValue, *PJsonValue;
}
