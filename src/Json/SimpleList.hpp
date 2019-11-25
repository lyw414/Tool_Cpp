#include <stdlib.h>
namespace LYW_CODE
{
    typedef struct _SimpleListNode
    {
        void * m_data;
        struct _SimpleListNode * m_next;
    }TSimpleListNode, * PSimpleListNode;

    class SimpleList
    {
    private:
        PSimpleListNode m_begin;
        PSimpleListNode m_end;
        int num;

    public:

        SimpleList ()
        {
            m_begin = m_end = NULL;
        }

        void push_back (  void * data )
        {
            
            PSimpleListNode node;
            node = ( PSimpleListNode )malloc ( sizeof ( TSimpleListNode ) );
            node -> m_next = NULL;
            node->m_data = data;
            if ( m_end == NULL )
            {
                m_begin = m_end = node;
                return;
            }
            m_end -> m_next = node;
            m_end = node;
        }

        PSimpleListNode  begin () const 
        {
            return m_begin;
        }

        PSimpleListNode end () const 
        {
            return m_end;
        }

    };
}
