#pragma once
#include <string.h>
#include <stdlib.h>
namespace LYW_CODE
{
    class AnyValue
    {
    private:
        void * m_data = NULL;

    public:
        AnyValue()
        {
            m_data = NULL;
        }

        template < typename T>
        AnyValue(T value)
        {
            m_data = NULL;
            (*this) = value;
        }

        ~AnyValue()
        {
            if (m_data != NULL)
            {
                ::free(m_data);
                m_data = NULL;
            }
        }

        template < typename T >
        AnyValue & operator = (T value)
        {
            if (m_data != NULL)
            {
                ::free(m_data);
            }
            m_data = malloc(sizeof(value));
            memcpy(m_data, &value, sizeof(value));
            return *this;
        }

        template < typename T >
        operator T () const
        {
            return *((T*)m_data);
        }
    };

}

