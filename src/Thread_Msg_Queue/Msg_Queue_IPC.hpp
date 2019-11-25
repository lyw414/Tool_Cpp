#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <chrono>
#include <thread>
#include <string.h>
#include <chrono>
#include <unistd.h>
namespace LYW_CODE
{
    class Msg_Queue_IPC 
    {
    private:
        std::string m_key_word;
        //unused buffer's pointor  store in this queue
        int m_msgID_free;
        //push buffer's pointor to this queue to notify get data, after used the data should put pointor to free queue
        int m_msgID_used;

        void * m_data;
        int m_DataSize;
        int m_ArraySize;

    public:
        Msg_Queue_IPC( const std::string & key_word, int DataSize, int ArraySize )
        {
            key_t key;
            int fHandle;
            m_data = malloc ( DataSize * ArraySize ) ;
            m_key_word = key_word;
            void * pointor;

            //if file not exist, creat it 
            if ( ( fHandle = open ( ( m_key_word + "1").c_str(), O_CREAT | O_RDWR, 0666 ) ) > 0 )
            {
                close ( fHandle );
            }

            //free queue init 
            key =  ftok ( ( m_key_word + "1").c_str(), 'a' );
            

            m_msgID_free = msgget ( key, IPC_CREAT|IPC_EXCL|0666 );
            if ( m_msgID_free == -1 )
            {
                //free it and creat
                m_msgID_free = msgget ( key, IPC_CREAT|0666 );
                msgctl ( m_msgID_free,IPC_RMID,NULL );
                m_msgID_free = msgget ( key, IPC_CREAT|IPC_EXCL|0666 );
            }
            //push addr to free queue 
            for ( int iLoop = 0; iLoop < ArraySize ; iLoop++ )
            {
                pointor = (char *)m_data + iLoop  * DataSize;
                msgsnd ( m_msgID_free, &pointor , 0, 0 );
            }

            //used queue init 
            if ( ( fHandle = open ( ( m_key_word + "2").c_str(), O_CREAT | O_RDWR, 0666 ) ) > 0 )
            {
                close ( fHandle );
            }

            key = ftok ( (m_key_word + "2").c_str(), 'a' );
            m_msgID_used = msgget ( key, IPC_CREAT | IPC_EXCL | 0666 );
            if ( m_msgID_used == -1 )
            {
                //free it and creat 
                m_msgID_used = msgget ( key, IPC_CREAT|0666 );
                msgctl ( m_msgID_used, IPC_RMID, NULL );
                m_msgID_used = msgget ( key, IPC_CREAT|IPC_EXCL|0666 );
            }
            m_DataSize = DataSize;
            m_ArraySize = ArraySize;

        }

        ~Msg_Queue_IPC()
        {
            // free msg queue
            if ( m_msgID_free != -1 )
            {
                msgctl ( m_msgID_free, IPC_RMID, NULL );
                m_msgID_free = -1;
            }

            if ( m_msgID_used != -1 )
            {
                msgctl ( m_msgID_used, IPC_RMID, NULL );
            }
        }


        /*
         * @brief get usefull pointor from free queue
         *
         * return NULL failed 
         *             success
         *
         */
        void * GetUsefullPointor ( )
        {
            void *  buffer = NULL;
            if ( msgrcv ( m_msgID_free, &buffer, 0, 0, IPC_NOWAIT ) == -1 )
            {
                return NULL;
            }
            else
            {
                return buffer;
            }

        }



        /*
         * &brief give used pointor back to free queue 
         *
         * return must success
         *
         */
        int GiveBackUsedPointor ( void * Data ) 
        {
            msgsnd ( m_msgID_free, &Data, 0, 0 );
            return 0;
        }
        
        /*
         * @brief put pointor to used queue
         *
         * @param[in] Data this pointor's value must come from GetUsedPointor
         *
         */
        int PutPointorToUsedQueue ( void * Data )
        {
            msgsnd ( m_msgID_used, &Data,0 , 0 );
            return 0;
        }

        int GetPonitorFromUsedQueue ( void * OutData, int timeout )
        {
            void * buffer = NULL;
            int times = timeout * 1000 / 500;
            if ( timeout < 0 )
            {
                if (  msgrcv ( m_msgID_used, &buffer, 0, 0, 0 ) == -1 )
                {
                    return -1;
                }
                else
                {
                    memcpy ( OutData, &buffer, sizeof ( void * ) );
                    return 1;
                }
            }
            else
            {
                
                while ( 1 )
                {
                    if ( msgrcv ( m_msgID_used, &buffer, sizeof ( void * ),0, IPC_NOWAIT )  == -1 )
                    {
                        std::this_thread::sleep_for ( std::chrono::milliseconds ( 500 ) );      
                        if ( ( --times ) <= 0 ) 
                        {
                            return -1;
                        }

                    }
                    else
                    {
                        memcpy ( OutData, &buffer, sizeof ( void * ) );
                        return 1;
                    }
                }

            }

        }

        /*
         * @brief IPC msgQueue does not have Timeout, only provide NOBLOCK or Return immediately 
         * Alarm can not be used for Multy Pthread, Timer is used, Fixed Time reading Data from noblocking MsgQueue, Fixed Time set 500 ms
         *
         *  @param[in] timeout -1 BLOCKING >0 Time out
         *
         *  return -1 Timeout 1 sucess
         */
        int MSGRcv ( void * OutData, int timeout) 
        {
            //get used pointor from free queue
            void * buffer = NULL;
            int times = timeout * 1000 / 500;
            if ( timeout < 0 )
            {
                if (  msgrcv ( m_msgID_used, &buffer, 0, 0, 0 ) == -1 )
                {
                    return -1;
                }
                else
                {
                    memcpy ( OutData, buffer, m_DataSize );
                    //give back 
                    GiveBackUsedPointor ( buffer );
                    return 1;
                }
            }
            else
            {
                
                while ( 1 )
                {
                    if ( msgrcv ( m_msgID_used, &buffer, 0,0, IPC_NOWAIT )  == -1 )
                    {
                        std::this_thread::sleep_for ( std::chrono::milliseconds ( 500 ) );      
                        if ( ( --times ) <= 0 ) 
                        {
                            return -1;
                        }

                    }
                    else
                    {
                        memcpy ( OutData, buffer, m_DataSize );
                        //give back
                        GiveBackUsedPointor ( buffer );
                        return 1;
                    }
                }

            }
        }

        /*
         * @brief  send Data, Get Usefull Pointor from 
         *
         */
        int MSGSnd ( void * Data )
        {
            void * buffer = NULL;            
            buffer = GetUsefullPointor ( );

            if ( buffer == NULL )
            {
                return -1;
            }
            else
            {
                //copy data to buffer
                memcpy ( buffer, Data, m_DataSize );
                //put pointor to used queue
                PutPointorToUsedQueue ( buffer );
                return 1;
            }

        }

    };
}
