#pragma once
#include <string.h>
#include <stdlib.h>
#include <iostream>
namespace LYW_CODE
{
    class ArrayList
    {
    private:
        void * m_DataArray;
        int m_DataSize;
        int m_ArraySize;
        int beginIndex;
        int endIndex;
    public:
        ArrayList ( int DataSize, int ArraySize )
        {
            m_DataSize = DataSize;
            m_ArraySize = ArraySize + 2;
            beginIndex = 0;
            endIndex = 1;
            m_DataArray = (char *)malloc ( m_ArraySize * m_DataSize );
        }

        ArrayList ( )
        {
            m_DataSize = 0;
            m_ArraySize = 0;
            beginIndex = 0;
            endIndex = 0;
            m_DataArray = NULL;
        }


        ~ArrayList ( )
        {
            if ( m_DataArray != NULL )
            {
                free ( m_DataArray );
                m_DataArray = NULL;
            }
        }

        int Init ( int DataSize, int ArraySize )
        {
            if ( m_DataArray != NULL )
            {
                free ( m_DataArray );
                m_DataArray = NULL;
            }

            m_DataSize = DataSize;
            m_ArraySize = ArraySize + 2;
            beginIndex = 0;
            endIndex = 1;
            m_DataArray = (char *)malloc ( m_ArraySize * m_DataSize );
            return 0;
        }

        int full()
        {
            if ( ( endIndex + 1 ) % m_ArraySize == beginIndex )
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        int empty()
        {
            if ( ( beginIndex + 1 ) % m_ArraySize == endIndex )
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        int size()
        {
            int ret = endIndex - beginIndex  - 1;
            if ( ret < 0 )
            {
                return ret + m_ArraySize;
            }
            else
            {
                return ret;
            }
        }

        int push_back(void * Data )
        {
            int next = ( endIndex + 1 ) % m_ArraySize;
            if ( next != beginIndex )
            {
                memcpy ( (char *)m_DataArray + endIndex * m_DataSize, Data, m_DataSize );
                endIndex = next;
                return 1;
            }
            else
            {
                return 0;
            }
            
        }

        int pop_front(void * Data)
        {
            int next = ( beginIndex + 1 ) % m_ArraySize;
            if ( next != endIndex )
            {
                memcpy ( Data, (char *)m_DataArray + next * m_DataSize, m_DataSize );
                beginIndex = next;
                return 1;
            }
            else
            {
                return 0;
            }
        }
        // return left num of DataArray
        int push_all(void * DataArray, int ArraySize)
        {
            int num = 0;
            if ( endIndex < beginIndex )
            {
                num = beginIndex - endIndex - 1;
                if ( num >= ArraySize )
                {
                    memcpy ( (char *)m_DataArray + endIndex * m_DataSize, DataArray, ArraySize * m_DataSize );
                    endIndex += ArraySize;
                    return 0;
                }
                else
                {
                    memcpy ( (char *)m_DataArray + endIndex * m_DataSize, DataArray, num * m_DataSize );
                    endIndex = beginIndex - 1 ;
                    return ArraySize - num;
                }
            }
            else
            {
                // endIndex to end - 1
                num = m_ArraySize - endIndex - 1;
                if ( num >= ArraySize )
                {
                    memcpy ( (char *)m_DataArray + endIndex * m_DataSize, DataArray, ArraySize * m_DataSize );
                    endIndex += ArraySize;
                    return 0;
                }
                
                // is full last one is unusefull
                if ( beginIndex == 0 )
                {
                    memcpy ( (char *)m_DataArray + endIndex * m_DataSize, DataArray, num * m_DataSize );
                    endIndex = m_ArraySize - 1;
                    return ArraySize - num;
                }

                // last one is usefull copy to end
                num++;
                memcpy ( (char *)m_DataArray + endIndex  * m_DataSize, DataArray, num * m_DataSize );
                if ( beginIndex - 1 >= ArraySize - num )
                {
                    memcpy ( m_DataArray, (char *)DataArray + num * m_DataSize, ( ArraySize - num ) * m_DataSize ) ;
                    endIndex =  ( ArraySize - num );
                    return 0;
                }
                else
                {
                    memcpy ( m_DataArray, (char *)DataArray + num * m_DataSize, ( beginIndex - 1 ) * m_DataSize ) ;
                    endIndex = beginIndex - 1;
                    return ArraySize - num - ( beginIndex - 1 );
                }
                
            }
        }

        int pop_all(void * DataArray)
        {
            int num = 0;
            if ( endIndex > beginIndex  )
            {
                num = endIndex - beginIndex - 1;
                memcpy ( (char *)DataArray, (char *)m_DataArray + ( beginIndex + 1 ) * m_DataSize,num * m_DataSize );
                beginIndex = endIndex - 1;
                return num;
            }
            else
            {
                num = m_ArraySize - beginIndex - 1;
                memcpy ( (char *)DataArray, (char *)m_DataArray + ( beginIndex + 1 ) * m_DataSize,num * m_DataSize );
                memcpy ( (char *)DataArray + num * m_DataSize, m_DataArray, endIndex * m_DataSize );
                num += endIndex ;
                beginIndex = endIndex - 1;
                return num;
            }
        }
    };
}
