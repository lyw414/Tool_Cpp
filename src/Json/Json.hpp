#include <unordered_map>
#include <map>
#include <exception>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include "SimpleList.hpp"

namespace LYW_CODE
{
    int index = 0;
    int begin = 0;
    int end = 0;
    int begin1 = 0;
    int end1 = 0;
    int StrLen = 0;
    const char * JsonStr;

    typedef enum _JsonValueType
    {
        UnInitType_Json, //uninit type when not for use or init, will be free
        IntType_Json,
        DoubleType_Json,
        StringType_Json,
        BoolType_Json,
        ArrayType_Json,
        JsonType_Json
    }TJsonValueType;
  
    TJsonValueType type;
    /*
     * brief  Anlysis Json To K-Tree
     *
     */
    class Json  
    {
    private:
        Json * m_fatherNode;
        //node info 
        TJsonValueType m_type;
        //std::unordered_map < std::string, Json * > m_childNode;
        //std::map < std::string, Json * > * m_childNode;
        //std::list <  Json * > m_childNode;
        SimpleList m_childNode;
        char * m_key;
        void * m_value;
        //this member is used for recode array's size;
        size_t m_LenOfValue;
        std::vector < Json * > m_ArrayData;

        
        /* 
         * brief Variable Type return 0 means Variable size 
         */
        int GetSize ( TJsonValueType Type )
        {
            switch ( Type  )
            {
                case IntType_Json :
                    return sizeof ( long );
                case DoubleType_Json :
                    return sizeof ( double );
                case StringType_Json :
                    return 0;
                case ArrayType_Json :
                    return 0;
                default :
                    return -1;
            }
        }

        /*
         * @brief  cover old value with new value
         *
         */
        inline int SetValue ( TJsonValueType Type,  size_t uiLenOfData, const void * Data)
        {
            if ( m_type == UnInitType_Json  )
            {

                m_type = Type;
                m_value = malloc ( uiLenOfData );
                m_LenOfValue = uiLenOfData;
                memcpy ( m_value, Data, m_LenOfValue );
                return m_LenOfValue;
            }
            
            m_type = Type;
            if ( m_LenOfValue != uiLenOfData )
            {
                ::free ( m_value );
                m_value = malloc ( uiLenOfData );
                m_value = realloc ( m_value, uiLenOfData );
            }
            memcpy ( m_value, Data, m_LenOfValue );
            return m_LenOfValue;
        }

        inline int SetValue1 ( )
        {
            if ( m_type == UnInitType_Json  )
            {
                m_type = type;
            }

            if ( m_LenOfValue != end1 - begin1 )
            {

                m_LenOfValue = end1 - begin1;
                ::free ( m_value );
                m_value = malloc ( end1 - begin1 );
            }
            memcpy ( m_value, JsonStr + begin1, m_LenOfValue );
            return 1;
        }
        
        /*
         * @brief get num str boolean null type, end flg should be : 
         *        for example :  {  "abc" : xxx : begin->a end->" index -> :
         * 
         * return 1 success
         *        -1 failed
         */
        int Get_Key (  )
        {
            //skip space and newlines 
            while ( JsonStr[index] == ' ' || JsonStr[index] == '\r'  )
            {
                index++;
                if ( index >= StrLen )
                {
                    return -1;
                }
            }
            
            if ( JsonStr[index] ==  ',' )
            {
                index++;
                if ( index >= StrLen )
                {
                    return -1;
                }
            }

            //skip space and newlines 
            while ( JsonStr[index] == ' ' || JsonStr[index] == '\r'  )
            {
                index++;
                if ( index >= StrLen )
                {
                    return -1;
                }
            }



            if ( JsonStr[index] != '"' )
            {
                return -1;
            }
            else
            {
                begin = ++index;
                while ( 1 )
                {
                    if ( index >= StrLen )
                    {
                        return -1;
                    }

                    if ( JsonStr[index] == ':' )
                    {
                        //traceback to last char equal to '\"'
                        end = index;
                        while ( 1 )
                        {
                            end--;
                            if ( end < begin )
                            {
                                return -1;
                            }
                            if ( JsonStr[end] == '"' )
                            {
                                index++;
                                if ( index >= StrLen )
                                {
                                    return -1;
                                }
                                return 1; 
                            }
                            else if ( JsonStr[end] == ' ' || JsonStr[end] == '\r' )
                            {
                                //space or enter should be skip
                                continue;
                            }
                            else
                            {
                                return -1;
                            }
                        }
                    }
                    index++;
                }
            }
        }
        /*
         * @brief get num str boolean null type, end flg should be , ] }
         *
         * 
         * return 1 success
         *        2 null
         *        3 true 
         *        4 false
         *        -1 failed
         */
        int Get_Value (  )
        {
            //skip space and newlines 
            while ( JsonStr[index] == ' ' || JsonStr[index] == '\r'  )
            {
                index++;
                if ( index >= StrLen )
                {
                    return -1;
                }
            }
            
            if ( JsonStr[index] ==  ',' )
            {
                index++;
                if ( index >= StrLen )
                {
                    return -1;
                }
            }

            //skip space and newlines 
            while ( JsonStr[index] == ' ' || JsonStr[index] == '\r'  )
            {
                index++;
                if ( index >= StrLen )
                {
                    return -1;
                }
            }

            switch ( JsonStr[index] )
            {
            case ',':
                if ( Next_Char (  )  < 0 )
                {
                    return -1;
                }
            case '"':
                //string
                begin = ++index;
                type = StringType_Json;
                while ( 1 )
                {
                    if ( index >= StrLen )
                    {
                        return -1;
                    }

                    if ( JsonStr[index] == ','||JsonStr[index] == '}' || JsonStr[index] == ']' )
                    {
                        //traceback to last char equal to '\"'
                        end = index;
                        while ( 1 )
                        {
                            end--;
                            if ( end < begin )
                            {
                                return -1;
                            }
                            if ( JsonStr[end] == '"' )
                            {
                                return 1; 
                            }
                            else if ( JsonStr[end] == ' ' || JsonStr[end] == '\r' )
                            {
                                //space or enter should be skip
                                continue;
                            }
                            else
                            {
                                return -1;
                            }
                        }
                    }
                    index++;
                }
                return -1;
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
                type = IntType_Json;
                begin = index;
                while ( 1 )
                {
                    if ( index >= StrLen )
                    {
                        return -1;
                    }
                    if ( JsonStr[index] >= '0' && JsonStr[index] <= '9' )
                    {
                        //must be number
                        index++;
                        continue;
                    }
                    else if ( JsonStr[index] == '.' )
                    {
                        // point only appear once
                        if ( type != DoubleType_Json )
                        {
                            type = DoubleType_Json;
                            index++;
                            continue;
                        }
                        else
                        {
                            return -1;
                        }
                    }
                    else
                    {
                        //num end 
                        end = index;
                        //end flg check 
                        while ( 1 )
                        {
                            if ( index >= StrLen )
                            {
                                return -1;
                            }
                            if ( JsonStr[index] == ',' || JsonStr[index] == ']' || JsonStr[index] == '}' )
                            {
                                return 1;
                            }
                            else if ( JsonStr[end] == ' ' || JsonStr[end] == '\r' )
                            {
                                index++;
                                continue;
                            }
                            return -1;
                        }
                    }
                }
                return -1;
            case 'T':
            case 't':
                //true
                begin = index;
                end = index + 3;
                if ( index + 3 >= StrLen )
                {
                    return -1;
                }
                if ( memcmp ("TRUE",&JsonStr[index], 4 ) != 0 && memcmp ("true",&JsonStr[index], 4 ) != 0 )
                {
                    return -1;
                }
                index+=3;
                //end type check 
                while ( 1 )
                {
                    index++;
                    if ( index >= StrLen )
                    {
                        return -1;
                    }
                    if ( JsonStr[index] == ' ' || JsonStr[index] == '\r' )
                    {
                        continue;
                    }
                    else if (  JsonStr[index] == ',' || JsonStr[index] == ']' || JsonStr[index] == '}' )
                    {
                        type = BoolType_Json;
                        return 3;
                    }
                    else
                    {
                        return -1;
                    }
               }
               return -1;
            case 'F':
            case 'f':
                //false
                begin = index;
                end = index + 4;
                if ( index + 4 >= StrLen )
                {
                    return -1;
                }
                if ( memcmp ("FALSE",&JsonStr[index], 5 ) != 0 && memcmp ("false",&JsonStr[index], 5 ) != 0 )
                {
                    return -1;
                }
                index+=4;
                //end type check 
                while ( 1 )
                {
                    index++;
                    if ( index >= StrLen )
                    {
                        return -1;
                    }
                    if ( JsonStr[index] == ' ' || JsonStr[index] == '\r' )
                    {
                        continue;
                    }
                    else if (  JsonStr[index] == ',' || JsonStr[index] == ']' || JsonStr[index] == '}' )
                    {
                        type = BoolType_Json;
                        return 4;
                    }
                    else
                    {
                        return -1;
                    }
               }
               return -1;
            case 'N':
            case 'n':
                //null just format check, 
                begin = index;
                end = index + 3;
                if ( index + 3 >= StrLen )
                {
                    return -1;
                }
                if ( memcmp ("NULL",&(JsonStr[index]), 4 ) != 0 && memcmp ("null",&(JsonStr[index]), 4 ) != 0 )
                {
                    return -1;
                }
                index+=3;
                //end type check 
                while ( 1 )
                {
                    index++;
                    if ( index >= StrLen )
                    {
                        return -1;
                    }
                    if ( JsonStr[index] == ' ' || JsonStr[index] == '\r' )
                    {
                        continue;
                    }
                    else if (  JsonStr[index] == ',' || JsonStr[index] == ']' || JsonStr[index] == '}' )
                    {
                        return 2;
                    }
                    else
                    {
                        return -1;
                    }
               }
               return -1;
            case '{':
               type = JsonType_Json;
               return 1;
                    
            case '[':
                type = ArrayType_Json;
                return 1;

            default :
                return -1;
            }
        }
        /*
         * &brief skip ' ' \r 
         *
         * success  1
         * failed  -1        
         */
        inline int Skip_Separator_Char (  )
        {
            while ( JsonStr[index] == ' ' || JsonStr[index] == '\r'  )
            {
                if ( index >= StrLen )
                {
                    return -1;
                }
                index++;
            }
            return 1;
        }

        inline int Next_Char (  )
        {
            index++;
            if ( index >= StrLen )
            {
                return -1;
            }
            return 1;
        }
       
    public:
        //Json ( const std::string strJson ) {}
        //Json ( const char * szStrJson ) {}
        //Json ( const char * sStrJson,int iLenOfStrJson ) {}
        //Json () {}
        //~Json ( ) {}
        
        //string ( const std::string strJson );
        //string ( const char * szStrJson );
        //string ( const char * sStrJson,int iLenOfStrJson );
        //
        //int to_string ( char * outStrJson );
        //int to_string ( std::string & outStrJson );

        Json ()
        {
            m_type = UnInitType_Json;
            m_key = NULL;
            m_value = NULL;
            //m_childNode = NULL;
        }

        ~Json ( ) 
        {
            //for ( auto & p : m_childNode ) 
            //{
            //    delete p;
            //}
            //m_childNode.clear();

            if ( m_value != NULL )
            {
                ::free ( m_value );
                m_value = NULL;
            }
            m_type = UnInitType_Json;
            for ( auto & p : m_ArrayData )
            {
                delete p;
            }
            m_ArrayData.clear();
        }

        void free ()
        {
            //for ( auto & p : m_childNode ) 
            //{
            //    delete p;
            //}
            //m_childNode.clear();

            if ( m_value != NULL )
            {
                ::free ( m_value );
                m_value = NULL;
            }
            
            m_type = UnInitType_Json;
            for ( auto & p : m_ArrayData )
            {
                delete p;
            }
            m_ArrayData.clear();
        }

        int set_key ( const std::string & key )
        {
            //memcpy ( m_key, key.c_str(), key.length());
        }
        int set_key ( const char * key , int len )
        {

            //memcpy ( m_key, key, len);
        }


        int set_key (  )
        {
            if ( m_key == NULL )
            {
                m_key = (char *)malloc ( 20 );
            }
            memcpy  ( m_key,JsonStr + begin, end - begin );
            memcpy  ( m_key,JsonStr + begin, end - begin );
        }

        int delete_node ( const std::string & key )
        {
            if ( m_type == JsonType_Json )
            {
            }

            if ( m_type == ArrayType_Json )
            {
                m_ArrayData.pop_back();
            }
            return 0;
        }

        int setFatherNode ( Json * fatherNode )
        {
            m_fatherNode = fatherNode;
            return 0;
        }
        

        Json & operator = ( char )
        {
            SetValue1 ( );
            return *this;
        }
        Json & operator = ( int Data )
        {
            SetValue ( IntType_Json, sizeof ( long ), &Data );
            return *this;
        }

        Json & operator = ( long Data )
        {
            SetValue ( IntType_Json, sizeof ( long ), &Data );
            return *this;
        }

        Json & operator = ( short Data )
        {
            SetValue ( IntType_Json, sizeof ( long ), &Data );
            return *this;
        }


        Json & operator = ( bool Data )
        {
            SetValue ( BoolType_Json, sizeof ( long ), &Data );
            return *this;
        }

        Json & operator = ( float Data )
        {
            SetValue ( DoubleType_Json, sizeof ( float ), &Data );
            return *this;
        }

        Json & operator = ( double Data )
        {
            SetValue ( DoubleType_Json, sizeof ( double ), &Data );
            return *this;
        }

        Json & operator = ( const std::string & Data )
        {
            SetValue ( StringType_Json, Data.length(), Data.c_str() );
            return *this;
        }

        Json & operator = ( const char * Data )
        {
            SetValue ( StringType_Json, strlen ( Data ), Data );
            return *this;
        }
        
        Json & operator = ( const Json & Data )
        {
            Json * tmp;
            PSimpleListNode  d = NULL ;
            switch ( Data.m_type )
            {
                case UnInitType_Json :
                    break;
                case JsonType_Json :
                    free();
                    m_type = Data.m_type;
                    d = Data.m_childNode.begin();

                    for ( ;d != NULL; d = d->m_next )
                    {
                        tmp = new Json;
                        //m_childNode[p.first] = tmp;
                        m_childNode.push_back(tmp);
                        tmp->set_key(((Json *)(d->m_data))->m_key,strlen ( ((Json *)(d->m_data))->m_key ) );
                        tmp->setFatherNode(this);
                        *tmp = * ((Json *)(d->m_data));
                    }

                    break;

                case ArrayType_Json :
                    free();
                    m_type = Data.m_type;
                    for ( auto & p : Data.m_ArrayData )
                    {
                        tmp = new Json;
                        m_ArrayData.push_back(tmp);
                        tmp->setFatherNode(this);
                        *tmp = *p;
                    }
                    break;

                default :
                    if ( Data.m_LenOfValue != 0 )
                    {
                        free();
                        //m_type = Data.m_type;
                        SetValue ( Data.m_type, Data.m_LenOfValue, Data.m_value );
                    }
            }
            return *this;
        }

        Json & operator [] ( const char d )
        {

            Json * res = NULL;
            res = new Json;
            //m_childNode.push_back ( res ) ;
            res->setFatherNode ( this );
            res->set_key ( JsonStr + begin,5 );
            return * ( res );
        }

        inline Json & GetJson ( )
        {
            Json * res = NULL;
            res = new Json;
            m_childNode.push_back ( res ) ;
            res->setFatherNode ( this );
            res->set_key (  );
            return * res;
        }

        inline Json & GetJson_Array ( )
        {
            //m_ArrayData.push_back( new Json () );
            //m_ArrayData[m_ArrayData.size() - 1 ]->setFatherNode ( this );
            //return * ( m_ArrayData[ m_ArrayData.size() - 1 ] );
            return * new Json();
        }


        Json & operator [] ( const char * key )
        {
            Json * res = NULL;
            PSimpleListNode tmp;
            //std::unordered_map < std::string, Json * > :: iterator  it;
            //std::map < std::string, Json * > :: iterator  it;
            if ( m_type == UnInitType_Json )
            {
                m_type = JsonType_Json;
            }

            if ( m_type == JsonType_Json )
            {
                //it = m_childNode->find ( key );
                //if ( it == m_childNode->end() ) 
                //{
                //    res = (*m_childNode)[key] = new Json;
                //    res->setFatherNode ( this );
                //    res->set_key ( key );
                //    return * ( res );
                //}
                //else
                //{
                //    return * (it->second);
                //}
                //tmp = m_childNode.begin ();
                //for ( ;tmp != NULL; tmp = tmp->m_next )
                //{
                //    if (memcmp ( ((Json *)(tmp->m_data))->m_key.c_str(), key, strlen ( key ) ) == 0 )
                //    {
                //        return *((Json *)(tmp->m_data));
                //    }
                //}

                res = new Json;
                m_childNode.push_back ( res ) ;
                res->setFatherNode ( this );
                res->set_key ( key,5 );
                return * ( res );
 
            }
            else
            {
                throw std::runtime_error ( "Json :: Type is not Json!" );
            }
        }

        Json & operator [] ( const std::string & key )
        {
            Json * res = NULL;
            PSimpleListNode tmp;
            //std::unordered_map < std::string, Json * > :: iterator  it;
            std::map < std::string, Json * > :: iterator  it;
            if ( m_type == UnInitType_Json )
            {
                m_type = JsonType_Json;
 
            }

            if ( m_type == JsonType_Json )
            {
                //tmp = m_childNode.begin ();
                //for ( ;tmp != NULL; tmp = tmp->m_next )
                //{
                //    if ( ((Json *)(tmp->m_data))->m_key == key )
                //    {
                //        return *((Json *)(tmp->m_data));
                //    }
                //}

                res = new Json;
                m_childNode.push_back ( res ) ;
                res->setFatherNode ( this );
                res->set_key ( key );
                return * ( res );
 

                //it = m_childNode->find ( key );
                //if ( it == m_childNode->end() ) 
                //{
                //    res = (*m_childNode)[key] = new Json;
                //    res->setFatherNode ( this );
                //    res->set_key ( key );
                //    return * ( res );
                //}
                //else
                //{
                //    return * (it->second);
                //}

            }
            else
            {
                throw std::runtime_error ( "Json :: Type is not Json!" );
            }
        }



        Json & operator [] ( int index )
        {
            if ( m_type == UnInitType_Json )
            {
                m_type = ArrayType_Json;
            }

            if ( m_type == ArrayType_Json )
            {
                if ( index < m_ArrayData.size() && index >= 0 )
                {
                    return * ( m_ArrayData[index] );
                }
                else if ( index == m_ArrayData.size() || index == -1 )
                {
                    m_ArrayData.push_back( new Json () );
                    m_ArrayData[m_ArrayData.size() - 1 ]->setFatherNode ( this );
                    return * ( m_ArrayData[ m_ArrayData.size() - 1 ] );
                }
                else 
                {
                    throw std::runtime_error ( "Json :: Array Index out of range!");
                }
            }
            return *this;
        }

        int size ( )
        {
            if ( m_type == ArrayType_Json )
            {
                return m_ArrayData.size();
            }
            if ( m_type == JsonType_Json )
            {
                //return m_childNode.size();
                return 0;
            }
            else
            {
                return 0;
            }
        }

        operator int () const 
        {
            if ( m_type == IntType_Json )
            {
                return *((long *)(m_value));
            }
            else if ( m_type == UnInitType_Json )
            {
                //farther node delete key
                m_fatherNode->delete_node ( std::string(m_key ));
            }
            else
            {
                throw std::runtime_error ( "Json :: Not Int Type");
            }
        }

        operator std::string()  const 
        {

            if ( m_type == StringType_Json )
            {
                return std::string ( (const char *)m_value, m_LenOfValue );
            }
            else
            {
                throw std::runtime_error ( "Json :: Not String Type");
            }
        }

        operator double () const 
        {
            if ( m_type == DoubleType_Json )
            {
                return *((double *)(m_value));
            }
            else
            {
                throw std::runtime_error ( "Json :: Not double Type");
            }
        }

        operator bool () const 
        {
            if ( m_type == BoolType_Json )
            {
                return *((bool *)(m_value));
            }
            else
            {
                throw std::runtime_error ( "Json :: Not double Type");
            }
        }
       
        static void dump ( const Json & js, std::string & JsonStr )
        {
            int iLoop = 0;
            
            switch ( js.m_type )
            {
            case JsonType_Json:
                JsonStr += "{";

                //for ( auto & p : js.m_childNode )
                //{
                //    iLoop++;
                //    JsonStr += "\"" + p->m_key+ "\":";
                //    dump ( *(p), JsonStr ) ;
                //    if ( iLoop != js.m_childNode.size() )
                //        JsonStr += ",";
                //}
                
                JsonStr += "}";
                return;
            case ArrayType_Json:
                JsonStr += "[";
                for ( auto & p : js.m_ArrayData )
                {
                    iLoop++;
                    dump ( *p, JsonStr ) ;
                    if ( iLoop != js.m_ArrayData.size() )
                        JsonStr += ",";
                }
                JsonStr += "]";
                return ;
            case IntType_Json:
                JsonStr += std::to_string ( *((int*)js.m_value) );
                return;
            case DoubleType_Json:
                JsonStr += std::to_string ( *((double *)js.m_value) );
                return;
            case StringType_Json:
                JsonStr += "\"" + std::string((const char *)js.m_value,js.m_LenOfValue) +"\"";
                return;
            case BoolType_Json:
                if ( ((char *)js.m_value)[0] == 0x00 )
                {
                    JsonStr += "false";
                }
                else
                {
                    JsonStr += "true";
                }
                return;
            case UnInitType_Json:
                JsonStr += "null";
                return;
            }
        }

        std::string dump ( )
        {
            std::string str;
            dump ( *this, str );
            return str;
        }

        //static Json parse ( const char * JsonStr )
        //{
        //    return parse ( JsonStr, strlen ( JsonStr ) );
        //}

        //static Json parse ( const std::string & JsonStr )
        //{
        //    return parse ( JsonStr.c_str(), JsonStr.length() );
        //}

        static void parse ( const char * m_JsonStr, int  m_StrLen )
        {
            static Json js_r;
            Json * js = &js_r;
            Json * js_child = NULL;
            std::vector < Json * > Stack;
            JsonStr = m_JsonStr;
            int res = 0;
    index = 0;
    begin = 0;
    end = 0;
    begin1 = 0;
    end1 = 0;
    StrLen = m_StrLen;

           
            if ( JsonStr == NULL || StrLen == 0 )
            {
                //return js_r;
                return ;
            }
            //skip separator char  space and newlines
            if ( js_r.Skip_Separator_Char (  ) < 0 )
            {
                //throw std::runtime_error ( "Json 1 :: JsonStr Format Error!" );
                return;
            }

            switch ( JsonStr[index] ) 
            {
            case '[' :
                //Array Field begin 
                js->m_type = ArrayType_Json;
                Stack.push_back ( js );
                if ( js_r.Next_Char (  ) < 0 )
                {
                    //throw std::runtime_error ( "Json 2 :: JsonStr Format Error!" );
                    return;
                }
                //Get Value from JsonStr
                break;
            case '{' :
                //Json Field begin
                js->m_type = JsonType_Json;
                Stack.push_back ( js );
                if ( js_r.Next_Char (  ) < 0 )
                {
                    //throw std::runtime_error ( "Json 3 :: JsonStr Format Error!" );
                    return;
                }
                break;
             default :
                printf ("%d %d %d %s\n", index, begin, end, JsonStr + index );
                //throw std::runtime_error ( "Json 4 :: JsonStr Format Error!" );
                return;
                
            }

            while ( 1 )
            {
                //skip separator char  space and newlines
                if ( js_r.Skip_Separator_Char (  ) < 0 )
                {
                    //throw std::runtime_error ( "Json 5 :: JsonStr Format Error!" );
                    return;
                }
                switch ( JsonStr[index] ) 
                {
                case '[' :
                    //Array Field begin 
                    if ( js->m_type == ArrayType_Json )
                    {
                        js = &(js->GetJson_Array() );
                    }
                    else if ( js->m_type == JsonType_Json )
                    {
                        //js = &((*js)[std::string (JsonStr + begin, end - begin)]);
                        js = &(js->GetJson());
                    }
                    else
                    {
                        //throw std::runtime_error ( "Json 6 :: JsonStr Format Error!" );
                        return;
                    }
 
                    js->m_type = ArrayType_Json;
                    Stack.push_back ( js );
                    if ( js_r.Next_Char (  ) < 0 )
                    {
                        //throw std::runtime_error ( "Json 7 :: JsonStr Format Error!" );
                        return;
                    }
                    //next char can't be ,
                    //skip separator char  space and newlines
                    if ( js_r.Skip_Separator_Char (  ) < 0 )
                    {
                        //throw std::runtime_error ( "Json 8 :: JsonStr Format Error!" );
                        return;
                    }
 
                    if ( JsonStr[index] == ',' )
                    {
                        //throw std::runtime_error ( "Json 9 :: JsonStr Format Error!" );
                        return;
                    }
                    //Get Value from JsonStr
                    break;
                case '{' :
                    //Json Field begin
                    if ( js->m_type == ArrayType_Json )
                    {
                        js = &(js->GetJson_Array());
                    }
                    else if ( js->m_type == JsonType_Json )
                    {
                        //js = &((*js)[std::string (JsonStr + begin, end - begin)]);
                        js = &(js->GetJson());
                   }
                    else
                    {
                        //throw std::runtime_error ( "Json 10 :: JsonStr Format Error!" );
                        return;
                    }
 
                    js->m_type = JsonType_Json;
                    Stack.push_back ( js );
                    if ( js_r.Next_Char (  ) < 0 )
                    {
                        //throw std::runtime_error ( "Json 11 :: JsonStr Format Error!" );
                        return;
                    }
                    //next char can't be ,
                    //skip separator char  space and newlines
                    if ( js_r.Skip_Separator_Char (  ) < 0 )
                    {
                        //throw std::runtime_error ( "Json 12 :: JsonStr Format Error!" );
                        return;
                    }
 
                    if ( JsonStr[index] == ',' )
                    {
                        //throw std::runtime_error ( "Json 13 :: JsonStr Format Error!" );
                        return;
                    }
                    break;
                case ']' :
                    //Array Field end
                    if ( js -> m_type == ArrayType_Json )
                    {
                        Stack.pop_back ();
                        if ( Stack.empty() )
                        {
                            //end check
                            while ( 1 )
                            {
                                index++;
                                if ( index >= StrLen )
                                {
                                    //return js_r;
                                    return ;
                                }
                                
                                if ( JsonStr[index] == ' ' || JsonStr[index] == '\r'  )
                                {
                                    continue;
                                }
                                else
                                {
                                    throw std::runtime_error ( "Json 14 :: JsonStr Format Error!" );
                                    return;
                                }

                            }
                        }
                        else
                        {
                            //pop father node continue
                            js = Stack.back();
                            if ( js_r.Next_Char (  ) < 0 )
                            {
                                throw std::runtime_error ( "Json 15 :: JsonStr Format Error!" );
                                return;
                            }
                        }
                    }
                    else
                    {
                        throw std::runtime_error ( "Json 16 :: JsonStr Format Error!" );
                        return;
                    }
                    break;
                case '}' :
                    //Json Field begin
                    if ( js -> m_type == JsonType_Json )
                    {
                        Stack.pop_back ();
                        if ( Stack.empty() )
                        {
                            //end check
                            while ( 1 )
                            {
                                index++;
                                if ( index >= StrLen )
                                {
                                    //return js_r;
                                    return ;
                                }
                                
                                if ( JsonStr[index] == ' ' || JsonStr[index] == '\r'  )
                                {
                                    continue;
                                }
                                else
                                {
                                    throw std::runtime_error ( "Json 17 :: JsonStr Format Error!" );
                                    return;
                                }

                            }
                        }
                        else
                        {
                            //pop father node continue
                            js = Stack.back();
                            if ( js_r.Next_Char ( ) < 0 )
                            {
                                throw std::runtime_error ( "Json 18 :: JsonStr Format Error!" );
                                return;
                            }
 
                        }
                    }
                    else
                    {
                        throw std::runtime_error ( "Json 19 :: JsonStr Format Error!" );
                        return;
                    }
                    break;
                default :
                    //Value Field 
                    switch ( js->m_type )
                    {
                    case JsonType_Json :
                        //Get key 
                        if ( js_r.Get_Key (  ) < 0 )
                        {
                            throw std::runtime_error ( "Json 20 :: JsonStr Format Error!" );
                            return;
                        }
                        //get Value 
                        if ( ( res = js_r.Get_Value (  ) ) < 0 ) 
                        {
                            throw std::runtime_error ( "Json 21 :: JsonStr Format Error!" );
                            return;
                        }
                        else if ( res == 2 )
                        {
                            continue;
                        }

                        //set Value 
                        switch ( type )
                        {
                            case IntType_Json :
                                //(*js)[std::string(JsonStr + begin, end - begin)] = atoi ( JsonStr + begin1 );
                                //(*js)[JsonStr + begin] = atoi ( JsonStr + begin1 );
                                atoi ( JsonStr + begin1 );
                                js->GetJson().SetValue1();
                                break;
                            case DoubleType_Json :
                                //(*js)[std::string(JsonStr + begin, end - begin)] = atof ( JsonStr + begin1 );
                                atof ( JsonStr + begin1 );

                                js->GetJson().SetValue1();
                                break;
                            case StringType_Json :
                                //(*js)[std::string(JsonStr + begin, end - begin)] = std::string ( JsonStr + begin1, end1 - begin1 );
                                js->GetJson().SetValue1();
                                break;
                            case BoolType_Json :
                                switch ( res )
                                {
                                case 3 :
                                    //(*js)[std::string(JsonStr + begin, end - begin)] = true;
                                    js->GetJson().SetValue1();
                                break;
                                case 4 :
                                    //(*js)[std::string(JsonStr + begin, end - begin)] = false;
                                    js->GetJson().SetValue1();
                                break;
                                default :
                                    //throw std::runtime_error ( "Json 22 :: JsonStr Format Error!" );
                                    return;
                                }
                            case JsonType_Json:
                                break;
                            case ArrayType_Json:
                                break;
                            default :
                                //throw std::runtime_error ( "Json 23 :: JsonStr Format Error!" );
                                return;
                        }
                        break;
                    case ArrayType_Json :
                        //get Value 
                        if ( ( res = js_r.Get_Value (  ) ) < 0  )
                        {
                            //throw std::runtime_error ( "Json 24 :: JsonStr Format Error!" );
                            return;
                        }
                        else if ( res == 2 )
                        {
                            continue;
                        }

                        //set Value 
                        switch ( type )
                        {
                            case IntType_Json :
                                //(*js)[-1] = atoi ( JsonStr + begin1 );
                                atoi ( JsonStr + begin1 );
                                js->GetJson_Array().SetValue1();
                                break;
                            case DoubleType_Json :
                                //(*js)[-1] = atof ( JsonStr + begin1 );
                                 atof ( JsonStr + begin1 );
                                js->GetJson_Array().SetValue1();
                                break;
                            case StringType_Json :
                                //(*js)[-1] = std::string ( JsonStr + begin1, end1 - begin1 );
                                js->GetJson_Array().SetValue1();
                                break;
                            case JsonType_Json:
                                break;
                            case ArrayType_Json:
                                break;
                            case BoolType_Json :
                                switch ( res )
                                {
                                case 3 :
                                    js->GetJson_Array().SetValue1();
                                    break;
                                case 4 :
                                    js->GetJson_Array().SetValue1();
                                    break;
                                default :
                                    throw std::runtime_error ( "Json 25  :: JsonStr Format Error!" );
                                    return;
                                }
                                break;
                            default :
                                throw std::runtime_error ( "Json 26 :: JsonStr Format Error!" );
                                return;
                        }
                        break;
                    default :
                        throw std::runtime_error ( "Json 27 :: JsonStr Format Error!" );
                        return;
                    }
                }
            }
            //return js_r;
            return ;
        }
    };
}

