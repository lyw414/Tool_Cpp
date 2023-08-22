#include <pthread.h>
template <typename T>
class ThreadInstance
{
public:
    static __thread T * m_self;
    static T * GetInstance()
    {
        if (m_self == NULL)    
        {
            m_self = new T();
        }
        return m_self;
    }

};

template <typename T>
__thread T * ThreadInstance<T>::m_self = NULL;
