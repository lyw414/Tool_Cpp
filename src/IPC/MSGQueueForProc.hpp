#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include <errno.h>  

#include <string.h>

#include <string>

#include <stdio.h>
#include <stdlib.h>
#define PROC_MSG_QUEUE_MANAGER_LOCK_ID  0


namespace LYW_CODE
{
    class MSGQueueForProc
    {
    private:
        typedef struct _ProcMsgQueueInfo
        {
            int referenceCount;
            unsigned int blockSize;
            unsigned int msgNum;
        } ProcMsgQueueInfo_t;
        
        typedef struct _MyMsg
        {
            long mtype;
            int mtext;
        } MyMsg_t;


    public:
        MSGQueueForProc()
        {
            //memset(&m_MsgQueueInfo, 0x00, sizeof(ProcMsgQueueInfo_t));
            m_MsgQueueInfo = NULL;
            m_IsOpen = false;

            m_MsgQueueID = -1;
            m_BlockSize = 0;
            m_MsgNum = 0;

            /*Msg Queue Key*/
            m_KeyForShm = 0;
            m_ShmID = -1;

            m_KeyForOccupyMsq = 0;
            m_OccupyMsqID = -1;

            m_KeyForFreeMsq = 0;
            m_FreeMsqID = -1;

            m_KeyForProcLock = 0;
            m_ProcLockID = -1;
 
        }

        ~MSGQueueForProc()
        {
            close();
        }

        /**
         * @brief               open message queue! if not exist will create, otherwise open it
         *
         * @param msgQueueID    Message Queueu ID must be even!
         * @param blockSize     Max length of Message
         * @param msgNum        Max Number of Message for Cache
         *
         * @return      0       success create
         *              1       success open             
         *          !=0 1       failed  errno
         */
        int open(unsigned int msgQueueID, size_t blockSize, size_t msgNum)
        {
            int ret = 0;
            struct msqid_ds info;
            if (msgQueueID % 2 != 0 && msgQueueID != PROC_MSG_QUEUE_MANAGER_LOCK_ID && msgQueueID + 1 != PROC_MSG_QUEUE_MANAGER_LOCK_ID)
            {
                /*illegal msgQueueID, msgQueueID must be even and not be the same with aPROC_MSG_QUEUE_MANAGER_LOCK_ID*/
                return -1;
            }

            m_BlockSize = blockSize;

            m_MsgNum = msgNum;

            m_MsgQueueID = msgQueueID;

            if (!procLockCreate())
            {
                /*Creat proc Lock failed*/
                return -1;
            }

            memset(&info, 0x00, sizeof(struct msqid_ds));

            lock();
            /*Occupy Queue Create*/
            if ((m_KeyForOccupyMsq = ftok("/", msgQueueID)) < 0)
            {
                ret = -1;
                m_IsOpen = false;
                unlock();
                return ret;
            }

            if ((m_OccupyMsqID = msgget(m_KeyForOccupyMsq, IPC_CREAT | 0666)) < 0)
            {
                m_OccupyMsqID = -1;
                ret = -1;
                m_IsOpen = false;
                unlock();
                return ret;
            }

            /*shm create*/
            if ((m_KeyForShm = ftok("/", msgQueueID)) < 0)
            {
                ret = -1;
                m_IsOpen = false;
                unlock();
                return ret;
            }

            if ((m_ShmID = shmget(m_KeyForShm, sizeof(ProcMsgQueueInfo_t) + m_MsgNum * m_BlockSize, IPC_CREAT | 0666)) < 0)
            {
                ret = -1;
                m_IsOpen = false;
                unlock();
                return ret;
            }
            else
            {
                if ((void *)(m_MsgQueueInfo = (ProcMsgQueueInfo_t *)shmat(m_ShmID, NULL, 0)) == (void *)(-1))
                {
                    m_MsgQueueInfo = NULL;
                    ret = -1;
                    m_IsOpen = false;
                    unlock();
                    return ret;
                }
            }

            /*Free Message Queue Create*/
            if ((m_KeyForFreeMsq = ftok("/", msgQueueID + 1)) < 0)
            {
                ret = -1;
                m_IsOpen = false;
                unlock();
                return ret;
            }
            
            /*create Msg Queueu*/
            if ((m_FreeMsqID = msgget(m_KeyForFreeMsq, IPC_CREAT|IPC_EXCL|0666)) < 0)
            {
                if (errno == EEXIST)
                {
                    /*is MsgQueue exist, open it without init*/
                    if ((m_FreeMsqID = msgget(m_KeyForFreeMsq, 0666)) < 0)
                    {
                        ret = -1;
                        m_IsOpen = false;
                        unlock();
                        return ret;
                    }
                }
            }
            else
            {
                /*create New Msg Queue, Need Init it*/
                if ((m_FreeMsqID = msgget(m_KeyForFreeMsq, IPC_CREAT | 0666)) < 0)
                {
                    ret = -1;
                    m_IsOpen = false;
                    unlock();
                    return ret;
                }

                /*init*/
                memset(&info, 0x00, sizeof(msqid_ds));
                msgctl(m_FreeMsqID, IPC_STAT, &info);
                m_MsgNum = info.msg_qbytes / sizeof(void *);
                if (m_MsgNum > msgNum)
                {
                    m_MsgNum = msgNum;
                    info.msg_qbytes = m_MsgNum * sizeof(void *);
                }

                msgctl(m_OccupyMsqID, IPC_STAT, &info);
                info.msg_qbytes = m_MsgNum * sizeof(void *);
                msgctl(m_OccupyMsqID, IPC_SET, &info);
 
                msgctl(m_FreeMsqID, IPC_SET, &info);

                MyMsg_t myMsg;
                myMsg.mtype = 1;

                for (size_t iLoop = 0; iLoop < m_MsgNum; iLoop++)
                {
                    myMsg.mtext = iLoop;
                    if (msgsnd(m_FreeMsqID, &myMsg, sizeof(int), IPC_NOWAIT) < 0)
                    {
                        printf("Add Failed [%d] [%d]\n",iLoop, errno);
                    }
                }

                m_MsgQueueInfo->referenceCount = 0;
                m_MsgQueueInfo->blockSize = m_BlockSize;
                m_MsgQueueInfo->msgNum = m_MsgNum;
            }

            m_MsgQueueInfo->referenceCount++;
            m_BlockSize = m_MsgQueueInfo->blockSize;
            m_MsgNum = m_MsgQueueInfo->msgNum;

            m_IsOpen = true;
            unlock();

            return ret;
        }

        /**
         * @brief               recv From Message queue
         *
         * @param data          out buffer for recv data
         * @param recvLen       size of out buffer, should more than blockSize! if less than blockSize will return error!
         * @param mode          0 blocking 1 no blocking
         *
         * @return    > 0       recv Message len
         *           <= 0       failed  errno
         */
        int recv(void * data, size_t recvLen, int mode = 0)
        {
            int len = 0;
            MyMsg_t myMsg;
            mode = mode == 0 ? mode : IPC_NOWAIT;
            if (m_IsOpen)
            {
                if (data == NULL)
                {
                    return -1;
                }
                
                len = m_BlockSize < recvLen ? m_BlockSize : recvLen;
                if (msgrcv(m_OccupyMsqID, &myMsg, sizeof(void *), 0, mode) < 0)
                {
                    return -1;
                }
                void * tmp = (char *)(m_MsgQueueInfo) + sizeof(ProcMsgQueueInfo_t) + m_BlockSize * myMsg.mtext;
                memcpy(data, tmp, len);
                if (msgsnd(m_FreeMsqID, &myMsg, sizeof(void *), 0) < 0)
                {
                    return len;
                }
            }
            return len;
        }

        /**
         * @brief               send To Message queue
         *
         * @param data          send Data
         * @param lenOfData     length of send data
         * @param mode          0 blocking 1 no blocking
         *
         * @return   > 0        send Message Len
         *          <= 0        failed errno
         */
        int send(void * data, size_t lenOfData, int mode = 1)
        {
            MyMsg_t myMsg;
            mode = mode == 0 ? mode : IPC_NOWAIT;
            if (m_IsOpen)
            {
                if (lenOfData > m_BlockSize)
                {
                    return -1;
                }

                /*get buffer*/
                if (msgrcv(m_FreeMsqID, &myMsg, sizeof(void *), 0, mode) < 0)
                {
                    return -1;
                }
                
                void * tmp = (char *)(m_MsgQueueInfo) + sizeof(ProcMsgQueueInfo_t) + m_BlockSize * myMsg.mtext;
                memcpy(tmp, data, lenOfData);

                if (msgsnd(m_OccupyMsqID, &myMsg, sizeof(void *), 0) < 0)
                {
                    return -1;
                }
            }
            return lenOfData;
        }

        /**
         * @brief               Message queue size
         *  
         * @return >= 0         message queue size
         *          < 0         failed errno
         */
        int size()
        {
            return 1;
        }

        /**
         * @brief               clear all Message not recv 
         *
         */
        void clear()
        {
            return;
        }

        /**
         * @brief               close message queue and free resource
         */
        void close()
        {
            lock();
            if (m_IsOpen == true)
            {
                if (m_MsgQueueInfo->referenceCount == 1)
                {
                    shmctl(m_ShmID, IPC_RMID, NULL); 
                    msgctl(m_OccupyMsqID, IPC_RMID, NULL); 
                    msgctl(m_FreeMsqID, IPC_RMID, NULL); 
                }
                else
                {
                    m_MsgQueueInfo->referenceCount--;
                }
                m_IsOpen = false;
            }
            unlock();
        }
        
        void release()
        {
            lock();
            shmctl(m_ShmID, IPC_RMID, NULL); 
            msgctl(m_OccupyMsqID, IPC_RMID, NULL); 
            msgctl(m_FreeMsqID, IPC_RMID, NULL); 
            m_IsOpen = false;
            unlock();
        }

    public:

    private:
        void lock()
        {
            MyMsg_t myMsg;
            msgrcv(m_ProcLockID, &myMsg, sizeof(void *), 0, 0);
 
        }

        void unlock()
        {
            MyMsg_t myMsg;
            myMsg.mtype = 1;
            myMsg.mtext = 1;
            msgsnd(m_ProcLockID, &myMsg, sizeof(void *), IPC_NOWAIT);
 
        }


        bool procLockCreate()
        {
            if ((m_KeyForProcLock = ftok("/", PROC_MSG_QUEUE_MANAGER_LOCK_ID)) < 0)
            {
                return false;
            }
            
            /*create Msg Queueu*/
            if ((m_ProcLockID = msgget(m_KeyForProcLock, IPC_CREAT|IPC_EXCL|0666)) < 0)
            {
                if (errno == EEXIST)
                {
                    /*is MsgQueue exist, open it without init*/
                    if ((m_ProcLockID = msgget(m_KeyForProcLock, 0666)) < 0)
                    {
                        return false;
                    }
                    return true;
                }
            }
            else
            {
                /*create New Msg Queue, Need Init it*/
                if ((m_ProcLockID = msgget(m_KeyForProcLock, IPC_CREAT | 0666)) < 0)
                {
                    return false;
                }

                /*init*/
                struct msqid_ds info;
                MyMsg_t myMsg;
                myMsg.mtype = 1;

                memset(&info, 0x00, sizeof(msqid_ds));

                msgctl(m_ProcLockID, IPC_STAT, &info);
                info.msg_qbytes = 4;
                msgctl(m_ProcLockID, IPC_SET, &info);
                
                if (msgsnd(m_ProcLockID, &myMsg, sizeof(void *), IPC_NOWAIT) < 0)
                {
                    return false;
                }
            }
            return true;
        }
        

                

    private:
        bool m_IsOpen;

        int m_UseCount;

        int m_MsgQueueID;
        size_t m_BlockSize;
        size_t m_MsgNum;

        /*Msg Queue Key*/
        key_t m_KeyForShm;
        int m_ShmID;

        key_t m_KeyForOccupyMsq;
        int m_OccupyMsqID;

        key_t m_KeyForFreeMsq;
        int m_FreeMsqID;

        key_t m_KeyForProcLock;
        int m_ProcLockID;

        ProcMsgQueueInfo_t * m_MsgQueueInfo;

        char ta[sizeof(void *)];
    };
}
