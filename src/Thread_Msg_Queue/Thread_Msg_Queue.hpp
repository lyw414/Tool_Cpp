#pragma once
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "ArrayList.hpp"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

namespace LYW_CODE
{
    class Thread_Msg_Queue 
    {
    private:
        std::mutex m_lock;
        std::condition_variable m_condition;
        std::mutex m_cv_lock;
        ArrayList m_DataArray;
        int m_msgID;

    public:
        Thread_Msg_Queue (  size_t ArraySize , size_t DataSize ) 
        {
            m_DataArray.Init ( DataSize, ArraySize );
        }

        ~Thread_Msg_Queue ( ) 
        {
            msgctl ( m_msgID, IPC_RMID, NULL );
        }

        int MsgSend ( void * Data )
        {
            int ret = 0;
            m_lock.lock();
            ret = m_DataArray.push_back ( Data );
            m_lock.unlock();
            m_condition.notify_one ();
            return ret;
        }
        /*
         * @brief Push DataArray to List if Left space not enougth return
         * left number of DataArray whitch are not pushed  to List
         *
         * optimization for Push Times
         */
        int MsgSend ( void * Data , int ArraySize )
        {
            int ret = 0;
            m_lock.lock();
            ret = m_DataArray.push_all ( Data, ArraySize );
            m_lock.unlock();
            m_condition.notify_one ();
            return ret;
        }
        

        /*
         * @brief  Get All Data In DataArray
         * 
         * @param[out] OutData : Out DataArray Pointor
         * @param[out] outArraySize : OutDataArray's Size 
         * @param[in]  Timeout : Recv Timeout, -1 means block untill receive data
         * 
         * return  -1 : TimeOut 
         *         <0 : RecvError
         *         >0 : Receive DataArray Size 
         */
        int MsgRecv( void * OutDataArray, int & outArraySize, int Timeout )
        {
            int ret = 0;
            m_lock.lock();
            ret = outArraySize = m_DataArray.pop_all( OutDataArray );
            m_lock.unlock();
            if ( ret == 0 )
            {
                //blocking  untill receive data
                std::unique_lock < std::mutex > lck(m_cv_lock);
                if ( Timeout == -1 )
                {
                    m_condition.wait(lck);
                    m_lock.lock();
                    ret = outArraySize = m_DataArray.pop_all( OutDataArray );
                    m_lock.unlock();
                    return ret;
                }

                if ( m_condition.wait_for(lck, std::chrono::seconds(Timeout)) == std::cv_status::timeout )
                {
                    return -1;
                }   
                else
                {

                    m_lock.lock();
                    ret = outArraySize = m_DataArray.pop_all( OutDataArray );
                    m_lock.unlock();
                    return ret;
                }
            }
            else
            {
                return ret;
            }
        }
        
        /*
         *
         * @brief only return front Data in DataList 
         * 
         * @param[out] OutData : Out Data Pointor
         * @param[in]  Timeout : Recv Timeout, -1 means block untill receive data
         * 
         * return  -1 : TimeOut 
         *         <0 : RecvError
         *          1 : Success 
         */
        int MsgRecv( void * OutData, int Timeout )
        {
            int ret = 0;

            m_lock.lock();
            ret = m_DataArray.pop_front( OutData );
            m_lock.unlock();
            if ( ret == 0 )
            {
                //blocking  untill receive data
                std::unique_lock < std::mutex > lck(m_cv_lock);
                if ( Timeout == -1 )
                {
                    m_condition.wait(lck);
                    m_lock.lock();
                    ret = m_DataArray.pop_front ( OutData );
                    m_lock.unlock();
                    return ret;
                }

                if ( m_condition.wait_for(lck, std::chrono::seconds(Timeout)) == std::cv_status::timeout )
                {
                    return -1;
                }   
                else
                {
                    m_lock.lock();
                    ret = m_DataArray.pop_all( OutData );
                    m_lock.unlock();
                    return ret;
                }
            }
            else
            {
                return ret;
            }
        }
    };
}

