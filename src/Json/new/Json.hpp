#pragma once
#include "JsonValue.hpp"
#include <stdexcept>
namespace LYW_CODE
{
	class Json
	{
	private:
		PJsonValue m_EntraceNode;
		PJsonValue m_DeepNode; // recode Deepth
		
		//parse use
		char * m_JsonStrBegin;
		char * m_JsonStrEnd;
		char * m_JsonStrIndex;
		char * m_Key_Begin;
		char * m_Key_End;
		char * m_Value_Begin;
		char * m_Value_End;
		std::vector < PJsonValue > m_Stack;
		PJsonValue tmpNode;

		void Next_Char()
		{
			m_JsonStrIndex++;
			if (m_JsonStrIndex > m_JsonStrEnd)
			{
				throw std::runtime_error("Json 1:: JsonStr Format Error");
			}
		}

		void Skip_Separator_Char()
		{
			while (1)
			{
				if (m_JsonStrIndex <= m_JsonStrEnd)
				{
				    switch (m_JsonStrIndex[0])
				    {
				    case ' ':
				    case '\r':
				    	m_JsonStrIndex++;
				    	continue;
				    default:
				    	break;
				    }
				    break;
                }
                else
                {

					throw std::runtime_error("Json 2:: JsonStr Format Error");
				}
			}
		}
		void StrFree ( void * str)
		{
			if ( str < m_JsonStrBegin || str > m_JsonStrEnd )
			{
				::free(str);
			}
		}

		void free( PJsonValue node )
		{

            PSimpleListNode_Json ListNode;
			switch (node->m_type)
			{
			case StringType_Json:
				StrFree(node->m_string->m_str);
				break;
			case JsonType_Json:
			{
				ListNode = node->m_json->m_ChildNode.begin();
				while (ListNode!= NULL)
				{
					StrFree(ListNode->sKey);
					free(ListNode->data);
					ListNode = ListNode->next;
				}
				node->m_json->m_ChildNode.clear();
				delete(node->m_json);
				break;
			}
			case ArrayType_Json:
				for (auto & p : node->m_array->m_ArrayNode)
				{
					free(p);
				}
				delete(node->m_array);
				break;
			}
			::free(node);
		}

		void free_child (PJsonValue node)
		{

			switch ( node->m_type )
			{
			case StringType_Json:
				StrFree(node->m_string->m_str);
				::free(node->m_string);
				break;
			case JsonType_Json:
			{
				PSimpleListNode_Json ListNode = node->m_json->m_ChildNode.begin();
				while (ListNode!= NULL)
				{
					StrFree(ListNode->sKey);
					free(ListNode->data);
					ListNode = ListNode->next;
				}
				node->m_json->m_ChildNode.clear();
				delete(node->m_json);
				break;
			}
			case ArrayType_Json:
				for (auto & p : node->m_array->m_ArrayNode)
				{
					free(p);
				}
				delete(node->m_array);
				break;
			}
			node->m_type = NewType_Json;
		}
		
		/*
		 * @brief get all key-value for json object untill new jsonobject or jsonobject end
		 */
		void Set_Key_Value()
		{
			while (1)
			{
				Skip_Separator_Char();
				//Get Key
				if (m_JsonStrIndex[0] != '"')
				{
					throw std::runtime_error("Json 3:: JsonStr Format Error");
				}
				Next_Char();
				m_Key_Begin = m_JsonStrIndex;
				while (1)
				{
					switch (m_JsonStrIndex[0])
					{
					case ':':
						m_Key_End = m_JsonStrIndex - 1;
						//backtrace untill '"' ,but only separator char can appear
						while (1)
						{
							if (m_Key_End < m_Key_Begin)
							{
								throw std::runtime_error("Json 4:: JsonStr Format Error");
							}
							switch (m_Key_End[0])
							{
							case ' ':
							case '\r':
								m_Key_End--;
								continue;
							case '"':
								break;
							default:
								throw std::runtime_error("Json 5:: JsonStr Format Error");
							}
							break;
						}
						break;
					default:
						m_JsonStrIndex++;
						continue;
					}
					break;
				} // end while finish key get

				//GetValue
				Next_Char();//skip ':'
				Skip_Separator_Char();
				//decide value type 
				m_Value_Begin = m_JsonStrIndex;
				switch (m_JsonStrIndex[0])
				{
				case '"':
					//Anlysis string
					m_Value_Begin = ++m_JsonStrIndex;
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 6:: JsonStr Foramt Error");
						}
						switch (m_JsonStrIndex[0])
						{
						case '}':
						case ',':
							//backtrace untill '"' but only separator char can appear
							m_Value_End = m_JsonStrIndex - 1;
							while (1)
							{
								if (m_Value_End < m_Value_Begin)
								{
									throw std::runtime_error("Json 7:: JsonStr Foramt Error");
								}
								switch (m_Value_End[0])
								{
								case '"':
									//add string key-value
									if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
									{
										*(void**)tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
										tmpNode = (PJsonValue)(*(void**)tmpNode);
									}
									else
									{
										free_child(tmpNode);
									}
									tmpNode->m_type = StringType_Json;
									tmpNode->m_string = (PString_Json)malloc(sizeof(TString_Json));
									tmpNode->m_string->m_len = m_Value_End - m_Value_Begin;
									tmpNode->m_string->m_str = m_Value_Begin;
									break;
								case ' ':
								case '\r':
									m_Value_End--;
									continue;
								default:
									throw std::runtime_error("Json 8:: JsonStr Format Error");
								}
								break;
							}//end while backtrace 
							break;
						default:
							m_JsonStrIndex++;
							continue;
						}
						break;
					}
					break;
				case '-':
					Next_Char();
				case '.':
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
					//Anlysis number
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 9:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case '.':
							Next_Char();
							//double type
							while (1)
							{
								if (m_JsonStrIndex > m_JsonStrEnd)
								{
									throw std::runtime_error("Json 10:: JsonStr Format Error!");
								}
								if (m_JsonStrIndex[0] >= '0' && m_JsonStrIndex[0] <= '9')
								{
									m_JsonStrIndex++;
									continue;
								}
								while (1)
								{
									if (m_JsonStrIndex > m_JsonStrEnd)
									{
										throw std::runtime_error("Json 11:: JsonStr Format Error!");
									}
									switch (m_JsonStrIndex[0])
									{
									case '}':
									case ',':
										//add string key-value
										if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
										{
											*(void**)tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
											tmpNode = (PJsonValue)(*(void**)tmpNode);
										}
										else
										{
											free_child(tmpNode);
										}
										tmpNode->m_type = DoubleType_Json;
										tmpNode->m_double = ::atof(m_Value_Begin);
										break;
									case ' ':
									case '\r':
										m_JsonStrIndex++;
										continue;
									default:
										throw std::runtime_error("Json 12:: JsonStr Format Error");
									}
									break;
								}
								break;
								//end check
							}// finish double type anlysis
							break;
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
							m_JsonStrIndex++;
							continue;
						default:
							while (1)
							{
								if (m_JsonStrIndex > m_JsonStrEnd)
								{
									throw std::runtime_error("Json 13:: JsonStr Format Error!");
								}
								switch (m_JsonStrIndex[0])
								{
								case '}':
								case ',':
									//add string key-value
									if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
									{
										*(void**)tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
										tmpNode = (PJsonValue)(*(void**)tmpNode);
									}
									else
									{
										free_child(tmpNode);
									}
									tmpNode->m_type = IntType_Json;
									tmpNode->m_int = ::atoi(m_Value_Begin);
									break;
								case ' ':
								case '\r':
									m_JsonStrIndex++;
									continue;
								default:
									throw std::runtime_error("Json 14:: JsonStr Format Error");
								}
								break;
							}// end while finish int type Anlysis
							break;
						}
						break;
					}// end while num type Anlysis finished
					break;
				case 't':
					//true
					if (m_JsonStrIndex + 3 > m_JsonStrEnd && memcmp("true", m_Value_Begin, 4) != 0)
					{
						throw std::runtime_error("Json 15:: JsonStr Format Error");
					}
					//add string key-value
					if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
					{
						*(void**)tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
						tmpNode = (PJsonValue)(*(void**)tmpNode);
					}
					else
					{
						free_child(tmpNode);
					}
					tmpNode->m_type = BoolType_Json;
					tmpNode->m_bool = true;
					m_JsonStrIndex += 4;

					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 16:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case '}':
						case ',':
							break;
						case ' ':
						case '\r':
							m_JsonStrIndex++;
							continue;
						default:
							throw std::runtime_error("Json 17:: JsonStr Format Error");
						}
						break;
					}// end while end check 
					break;
				case 'f':
					//false
					if (m_JsonStrIndex + 4 > m_JsonStrEnd && memcmp("false", m_Value_Begin, 5) != 0)
					{
						throw std::runtime_error("Json 18:: JsonStr Format Error");
					}
					//add string key-value
					if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
					{
						*(void**)tmpNode =  (PJsonValue)malloc(sizeof(TJsonValue));
						tmpNode = (PJsonValue)(*(void**)tmpNode);
					}
					else
					{
						free_child(tmpNode);
					}
					tmpNode->m_type = BoolType_Json;
					tmpNode->m_bool = false;

					m_JsonStrIndex += 5;
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 19:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case '}':
						case ',':
							break;
						case ' ':
						case '\r':
							m_JsonStrIndex++;
							continue;
						default:
							throw std::runtime_error("Json 20:: JsonStr Format Error");
						}
						break;
					}// end while end check 
					break;
				case 'n':
					//false
					if (m_JsonStrIndex + 3 > m_JsonStrEnd && memcmp("null", m_Value_Begin, 4) != 0)
					{
						throw std::runtime_error("Json 21:: JsonStr Format Error");
					}
					//add string key-value
					if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
					{
						*(void**)tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
						tmpNode = (PJsonValue)(*(void**)tmpNode);
					}
					else
					{
						free_child(tmpNode);
					}
					tmpNode->m_type = NullType_Json;
					m_JsonStrIndex += 4;
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 23:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case '}':
						case ',':
							break;
						case ' ':
						case '\r':
							m_JsonStrIndex++;
							continue;
						default:
							throw std::runtime_error("Json 24:: JsonStr Format Error");
						}
						break;
					}// end while end check 
					break;
				case '{':
				case '[':
					break;
				default:
					throw std::runtime_error("Json 25:: JsonStr Format Error");
				}
				if (m_JsonStrIndex[0] == ',')
				{
					Next_Char();
					continue;
				}
				break;
			}
		}
		/*
		 * @brief get all value for json object untill new jsonobject or jsonobject end
		 */
		void Set_Array_Value()
		{
			while (1)
			{
				Skip_Separator_Char();
				//decide value type 
				m_Value_Begin = m_JsonStrIndex;
				switch (m_JsonStrIndex[0])
				{
				case '"':
					//Anlysis string
					m_Value_Begin = ++m_JsonStrIndex;
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 26:: JsonStr Foramt Error");
						}
						switch (m_JsonStrIndex[0])
						{
						case ']':
						case ',':
							//backtrace untill '"' but only separator char can appear
							m_Value_End = m_JsonStrIndex - 1;
							while (1)
							{
								if (m_Value_End < m_Value_Begin)
								{
									throw std::runtime_error("Json 27:: JsonStr Foramt Error");
								}
								switch (m_Value_End[0])
								{
								case '"':
									//add string key-value
									tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
									tmpNode->m_type = StringType_Json;
									tmpNode->m_string = (PString_Json)malloc(sizeof(TString_Json));
									tmpNode->m_string->m_len = m_Value_End - m_Value_Begin;
									tmpNode->m_string->m_str = m_Value_Begin;
									m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);
									break;
								case ' ':
								case '\r':
									m_Value_End--;
									continue;
								default:
									throw std::runtime_error("Json 28:: JsonStr Format Error");
								}
								break;
							}//end while backtrace 
							break;
						default:
							m_JsonStrIndex++;
							continue;
						}
						break;
					}
					break;
				case '-':
					Next_Char();
				case '.':
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
					//Anlysis number
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 29:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case '.':
							Next_Char();
							//double type
							while (1)
							{
								if (m_JsonStrIndex > m_JsonStrEnd)
								{
									throw std::runtime_error("Json 30:: JsonStr Format Error!");
								}
								if (m_JsonStrIndex[0] >= '0' && m_JsonStrIndex[0] <= '9')
								{
									m_JsonStrIndex++;
									continue;
								}
								while (1)
								{
									if (m_JsonStrIndex > m_JsonStrEnd)
									{
										throw std::runtime_error("Json 31:: JsonStr Format Error!");
									}
									switch (m_JsonStrIndex[0])
									{
									case ']':
									case ',':
										tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
										tmpNode->m_type = DoubleType_Json;
										tmpNode->m_double = ::atof(m_Value_Begin);
										m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);
										break;
									case ' ':
									case '\r':
										m_JsonStrIndex++;
										continue;
									default:
										throw std::runtime_error("Json 32:: JsonStr Format Error");
									}
									break;
								}
								break;
								//end check
							}// finish double type anlysis
							break;
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
							m_JsonStrIndex++;
							continue;
						default:
							while (1)
							{
								if (m_JsonStrIndex > m_JsonStrEnd)
								{
									throw std::runtime_error("Json 33:: JsonStr Format Error!");
								}
								switch (m_JsonStrIndex[0])
								{
								case ']':
								case ',':
									tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
									tmpNode->m_type = IntType_Json;
									tmpNode->m_int = ::atoi(m_Value_Begin);
									m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);
									break;
								case ' ':
								case '\r':
									m_JsonStrIndex++;
									continue;
								default:
									throw std::runtime_error("Json 34:: JsonStr Format Error");
								}
								break;
							}// end while finish int type Anlysis
							break;
						}
						break;
					}// end while num type Anlysis finished
					break;
				case 't':
					//true
					if (m_JsonStrIndex + 3 > m_JsonStrEnd && memcmp("true", m_Value_Begin, 4) != 0)
					{
						throw std::runtime_error("Json 35:: JsonStr Format Error");
					}

					tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
					tmpNode->m_type = BoolType_Json;
					tmpNode->m_bool = true;
					m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);
					m_JsonStrIndex += 4;
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 36:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case ']':
						case ',':
							break;
						case ' ':
						case '\r':
							m_JsonStrIndex++;
							continue;
						default:
							throw std::runtime_error("Json 37:: JsonStr Format Error");
						}
						break;
					}// end while end check 
					break;
				case 'f':
					//false
					if (m_JsonStrIndex + 4 > m_JsonStrEnd && memcmp("false", m_Value_Begin, 5) != 0)
					{
						throw std::runtime_error("Json 38:: JsonStr Format Error");
					}

					tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
					tmpNode->m_type = BoolType_Json;
					tmpNode->m_bool = false;
					m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);

					m_JsonStrIndex += 5;
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 39:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case ']':
						case ',':
							break;
						case ' ':
						case '\r':
							m_JsonStrIndex++;
							continue;
						default:
							throw std::runtime_error("Json 40:: JsonStr Format Error");
						}
						break;
					}// end while end check 
					break;
				case 'n':
					//null
					if (m_JsonStrIndex + 3 > m_JsonStrEnd && memcmp("null", m_Value_Begin, 4) != 0)
					{
						throw std::runtime_error("Json 41:: JsonStr Format Error");
					}

					tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
					tmpNode->m_type = NullType_Json;
					m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);

					m_JsonStrIndex += 4;
					while (1)
					{
						if (m_JsonStrIndex > m_JsonStrEnd)
						{
							throw std::runtime_error("Json 42:: JsonStr Format Error!");
						}
						switch (m_JsonStrIndex[0])
						{
						case ']':
						case ',':
							break;
						case ' ':
						case '\r':
							m_JsonStrIndex++;
							continue;
						default:
							throw std::runtime_error("Json 43:: JsonStr Format Error");
						}
						break;
					}// end while end check 
					break;
				case '{':
				case '[':
					break;
				default:
					throw std::runtime_error("Json 44:: JsonStr Format Error");
				}
				if (m_JsonStrIndex[0] == ',')
				{
					Next_Char();
					continue;
				}
				break;
			}
		}
	public:
		Json()
		{
			m_DeepNode = m_EntraceNode = (PJsonValue)malloc(sizeof(TJsonValue));
			m_EntraceNode->m_type = NewType_Json;
			m_JsonStrBegin = NULL;
		}
		Json & parse(const char * JsonStr, int StrLen)
		{

			if (JsonStr == NULL || StrLen <= 0)
			{
				return *this;
			}
			//clear all data
			//free_child(m_DeepNode);
			//m_DeepNode = m_EntraceNode = (PJsonValue)malloc(sizeof(TJsonValue));
			//m_EntraceNode->m_type = NewType_Json;
            if ( m_JsonStrBegin != NULL )
            {
                ::free(m_JsonStrBegin);
            }
			m_Stack.clear();
			m_JsonStrIndex = m_JsonStrBegin = (char *)malloc(StrLen);
			memcpy(m_JsonStrBegin, JsonStr, StrLen);
			m_JsonStrEnd = m_JsonStrBegin + StrLen - 1;
			m_DeepNode = m_EntraceNode;

			Skip_Separator_Char();
			//make sure Json Entrance Type
			switch (m_JsonStrIndex[0])
			{
			case '[':
				m_DeepNode->m_type = ArrayType_Json;
				m_DeepNode->m_array = new TArrayObject_Json;
				m_Stack.push_back(m_DeepNode);
				Next_Char();
				break;
			case '{':
				m_DeepNode->m_type = JsonType_Json;
				m_DeepNode->m_json = new TJsonObject_Json;
				m_Stack.push_back(m_DeepNode);
				Next_Char();
				break;
			default:
				return *this;
			}

			//while anlysis Json Str
			while (1)
			{
				Skip_Separator_Char();
				switch (m_JsonStrIndex[0])
				{
				case '[':
					if (m_DeepNode->m_type == JsonType_Json)
					{
						//create ArrayObject
						if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
						{
							//point to child field
							*(void**)tmpNode = m_DeepNode = (PJsonValue)malloc(sizeof(TJsonValue));
						}
						else
						{
							//point to child field
							free_child(tmpNode);
							m_DeepNode = tmpNode;
						}
						m_DeepNode->m_type = ArrayType_Json;
						m_DeepNode->m_array = new TArrayObject_Json;
						m_Stack.push_back(m_DeepNode);
						Next_Char(); //must have end field
					}
					else if (m_DeepNode->m_type == ArrayType_Json)
					{
						tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
						m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);
						tmpNode->m_type = ArrayType_Json;
						tmpNode->m_array = new TArrayObject_Json;
						m_Stack.push_back(tmpNode);
						//point to child field
						Next_Char(); //must have end field
						m_DeepNode = tmpNode;
					}
					else
					{
						std::runtime_error("Json 45:: JsonStr Format Error");
					}
					break;
				case '{':
					if (m_DeepNode->m_type == JsonType_Json)
					{
						//create ArrayObject
						if (!m_DeepNode->m_json->m_ChildNode.find_add(m_Key_Begin, m_Key_End - m_Key_Begin, (void **)&tmpNode))
						{
							//point to child field
							*(void**)tmpNode = m_DeepNode = (PJsonValue)malloc(sizeof(TJsonValue));
						}
						else
						{
							//point to child field
							free_child(tmpNode);
							m_DeepNode = tmpNode;
						}
						m_DeepNode->m_type = JsonType_Json;
						m_DeepNode->m_json = new TJsonObject_Json;
						m_Stack.push_back(m_DeepNode);
						Next_Char(); //must have end field
					}
					else if (m_DeepNode->m_type == ArrayType_Json)
					{
						tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
						m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);
						tmpNode->m_type = JsonType_Json;
						tmpNode->m_json = new TJsonObject_Json;
						m_Stack.push_back(tmpNode);
						Next_Char(); //must have end field
						//point to child field
						m_DeepNode = tmpNode;
					}
					else
					{
						throw std::runtime_error("Json 46:: JsonStr Format Error");
					}
					break;
				case ']':
					//ArrayObject end, check field end or field continue;
				case '}':
					//JsonObject end, check field end or field continue;
					//pop
					m_Stack.pop_back();
					if (m_Stack.empty())
					{
						//all field end, check end 
                        m_JsonStrIndex++;
						while (1)
						{
							if (m_JsonStrIndex >= m_JsonStrEnd)
							{
								m_DeepNode = m_EntraceNode;
								return *this;
							}
							switch (m_JsonStrIndex[0])
							{
							case ' ':
							case '\r':
								m_JsonStrIndex++;
								continue;
							default:
								throw std::runtime_error("Json 47:: JsonStr Format Error");
							}
						}
					}
					else
					{
						//field continue, check ','
						m_DeepNode = m_Stack.back();
						Next_Char();
						Skip_Separator_Char();
						if (m_JsonStrIndex[0] == ',')
						{
							Next_Char();
						}
					}
					break;
				default :
					//value get
					switch (m_DeepNode->m_type)
					{
					case JsonType_Json:
						Set_Key_Value();
						break;
					case ArrayType_Json:
						Set_Array_Value();
						break;
					default :
						throw std::runtime_error("Json 48:: JsonStr Format Error");
					}
				}
			}
		}

		Json & operator [] (const char * key )
		{
			if (m_DeepNode->m_type == NewType_Json)
			{
				m_DeepNode->m_type = JsonType_Json;
				m_DeepNode->m_json = new TJsonObject_Json;
			}

			if (m_DeepNode->m_type != JsonType_Json)
			{
				m_DeepNode = m_EntraceNode;
				throw std::runtime_error("Json 49:: Json Type Not JsonObject");
			}

			if (!m_DeepNode->m_json->m_ChildNode.find_add_copy((char *)key, strlen(key), (void **)&tmpNode))
			{
				//not found add newType_Json
				*(void**)tmpNode = m_DeepNode = (PJsonValue)malloc(sizeof(TJsonValue));
				m_DeepNode->m_type = NewType_Json;
			}
			else
			{
				m_DeepNode = tmpNode;
			}
			return *this;
		}

		Json & operator[] (int index)
		{
			if (m_DeepNode->m_type == NewType_Json)
			{
				m_DeepNode->m_type = ArrayType_Json;
				m_DeepNode->m_array = new TArrayObject_Json;
			}

			if (m_DeepNode->m_type != ArrayType_Json)
			{
				m_DeepNode = m_EntraceNode;
				throw std::runtime_error("Json 50:: Json Type Not JsonObject");
			}

			if (index < m_DeepNode->m_array->m_ArrayNode.size() && index >= 0)
			{
				m_DeepNode = m_DeepNode->m_array->m_ArrayNode[index];
			}
			else if (index == -1 )
			{
				//add one 
				tmpNode = (PJsonValue)malloc(sizeof(TJsonValue));
				tmpNode->m_type = NewType_Json;
				m_DeepNode->m_array->m_ArrayNode.push_back(tmpNode);
				m_DeepNode = tmpNode;
			}
			else
			{
				m_DeepNode = m_EntraceNode;
				throw std::runtime_error("Json 51:: ArrayObject Index out of Range");
			}
			return *this;
		}

		Json & operator = (int data)
		{
			free_child(m_DeepNode);
			m_DeepNode->m_type = IntType_Json;
			m_DeepNode->m_int = data;
			m_DeepNode = m_EntraceNode;
			return *this;
		}

		Json & operator = (double data)
		{
			free_child(m_DeepNode);
			m_DeepNode->m_type = DoubleType_Json;
			m_DeepNode->m_double = data;
			m_DeepNode = m_EntraceNode;
			return *this;
		}

		Json & operator = (bool data)
		{
			free_child(m_DeepNode);
			m_DeepNode->m_type = BoolType_Json;
			m_DeepNode->m_bool = data;
			m_DeepNode = m_EntraceNode;
			return *this;
		}

		Json & operator = (const char * data)
		{
			free_child(m_DeepNode);
			m_DeepNode->m_type = StringType_Json;
			m_DeepNode->m_string = (PString_Json)malloc(sizeof(TString_Json));
			m_DeepNode->m_string->m_len = strlen(data);
			m_DeepNode->m_string->m_str = (char *)malloc( m_DeepNode->m_string->m_len);
			memcpy(m_DeepNode->m_string, data, m_DeepNode->m_string->m_len);
			m_DeepNode = m_EntraceNode;
			return *this;
		}

		Json & operator = (const std::string & data)
		{
			free_child(m_DeepNode);
			m_DeepNode->m_type = StringType_Json;
			m_DeepNode->m_string = (PString_Json)malloc(sizeof(TString_Json));
			m_DeepNode->m_string->m_len = data.length();
			m_DeepNode->m_string->m_str = (char *)malloc( m_DeepNode->m_string->m_len);
			memcpy(m_DeepNode->m_string, data.c_str(), m_DeepNode->m_string->m_len);
			m_DeepNode = m_EntraceNode;
			return *this;
		}

		operator int()
		{
			if (m_DeepNode->m_type == NewType_Json)
			{
				tmpNode = m_DeepNode;
				m_DeepNode = m_EntraceNode;
				::free(tmpNode);
				throw std::runtime_error("Json 52:: Json Type not Int");
			}
	
			if (m_DeepNode->m_type != IntType_Json)
			{
				m_DeepNode = m_EntraceNode;
				throw std::runtime_error("Json 53:: Json Type not Int");
			}
			int tmp = m_DeepNode->m_int;
			m_DeepNode = m_EntraceNode;
			return tmp;
		}

		operator double()
		{
			if (m_DeepNode->m_type == NewType_Json)
			{
				tmpNode = m_DeepNode;
				m_DeepNode = m_EntraceNode;
				::free(tmpNode);
				throw std::runtime_error("Json 54:: Json Type not Double");
			}
			if (m_DeepNode->m_type != DoubleType_Json)
			{
				throw std::runtime_error("Json 55:: Json Type not Double");
				m_DeepNode = m_EntraceNode;
			}
			double tmp = m_DeepNode->m_double;
			m_DeepNode = m_EntraceNode;
			return tmp;
		}

		operator bool ()
		{
			if (m_DeepNode->m_type == NewType_Json)
			{
				tmpNode = m_DeepNode;
				m_DeepNode = m_EntraceNode;
				::free(tmpNode);
				throw std::runtime_error("Json 56:: Json Type not bool");
			}
	
			if (m_DeepNode->m_type != BoolType_Json)
			{
				m_DeepNode = m_EntraceNode;
				throw std::runtime_error("Json 57:: Json Type not Bool");
			}
			bool tmp = m_DeepNode->m_bool;
			m_DeepNode = m_EntraceNode;
			return tmp;
		}

		operator std::string ()
		{
			if (m_DeepNode->m_type == NewType_Json)
			{
				tmpNode = m_DeepNode;
				m_DeepNode = m_EntraceNode;
				::free(tmpNode);
				throw std::runtime_error("Json 58:: Json Type not bool");
			}
	
			if (m_DeepNode->m_type != StringType_Json)
			{
				m_DeepNode = m_EntraceNode;
				throw std::runtime_error("Json 59:: Json Type not Bool");
			}
			std::string tmpStr(m_DeepNode->m_string->m_str,m_DeepNode->m_string->m_len);
			m_DeepNode = m_EntraceNode;
			return tmpStr;
		}






	};
}
