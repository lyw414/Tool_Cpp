#include <vector>
#include <list>
#include <stdlib.h>
#include <string.h>
namespace LYW_CODE
{
    typedef struct _Map_Node_Json
    {
        char * key;
        unsigned int len;
        void * data;
    } TMap_Node_Json, * PMap_Node_Json;

    class JsonStrHashMap
    {
    private:
        int m_size;
        void ** m_Bucket;
        PMap_Node_Json m_tmp;
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
            return hash  % m_size;
        }
    public:
        //std::vector < std::list < PMap_Node_Json > * > m_Bucket;
        JsonStrHashMap ( int size )  
        { 
            m_size = size;
            m_Bucket =(void **) malloc ( sizeof ( void * ) * size );
            memset ( m_Bucket, 0x00, sizeof ( void * ) * size );
        }
        void Insert_Without_Check ( const char * sKey, int len, void * data )
        {
            unsigned int key = APHash ( sKey, len );
            if ( m_Bucket[key] == NULL )
            {
                m_Bucket[key] = new std::list < PMap_Node_Json >;
            }
            m_tmp = (PMap_Node_Json)malloc( sizeof(TMap_Node_Json) );
            m_tmp->key = (char *)malloc ( len );
            //memset ( &m_tmp, 0x00, sizeof ( TMap_Node_Json ) );
            memcpy ( m_tmp->key, sKey, len );
            m_tmp->len = len;
            m_tmp->data = data;
            ((std::list < PMap_Node_Json > * )m_Bucket[key])->push_back( m_tmp );
        }

        bool Find_Add ( const char * sKey, int len, void ** data  )
        {
           unsigned int key = APHash ( sKey, len );
 
           if ( m_Bucket[key] == NULL )
           {
                m_Bucket[key] = new std::list < PMap_Node_Json > ;
           }
 
           for ( auto & p: *((std::list < PMap_Node_Json > * ) m_Bucket[ key ]) )
           {
               if ( p->len == len && memcmp ( p->key, sKey, len ) == 0 )
               {
                   *data = p->data;
                   return true;
               }
           }
           m_tmp = (PMap_Node_Json)malloc ( sizeof ( TMap_Node_Json ) );
           m_tmp->key = (char *)malloc ( len );
           //memset ( &m_tmp, 0x00, sizeof ( TMap_Node_Json ) );
           memcpy ( m_tmp->key, sKey, len );
           m_tmp->len = len;
           *data = &m_tmp->data;
           ( (std::list < PMap_Node_Json > * ) m_Bucket[key])->push_back( m_tmp );
           return false;
        }

        void * Find ( const char * sKey, int len  )
        {

           unsigned int key = APHash ( sKey, len );
 
           if ( m_Bucket[key] == NULL )
           {
                return NULL;
           }
 
           for ( auto & p: * ((std::list < PMap_Node_Json > * ) m_Bucket[ key ] ) )
           {
               if ( p->len == len && memcmp ( p->key, sKey, len ) == 0 )
               {
                   return p->data;
               }
           }
           return NULL;
        }
    };

