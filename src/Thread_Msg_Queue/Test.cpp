#include <thread>
#include <vector>
#include "Thread_Msg_Queue.hpp"
#include "Msg_Queue_IPC.hpp"
#include "MSGQueueForProc.hpp"
#include <sys/time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>


char key_word[] = "Test_lyw";
key_t key_msg;
int msgID;

LYW_CODE :: Thread_Msg_Queue arrayList ( 1024, 1024 );
LYW_CODE :: Msg_Queue_IPC msg_queue ( "./IPC", 1024, 128);
LYW_CODE :: MSGQueueForProc queue;
int * tolNum;
int flg = 1;
int sleep_time;

void outDo ( int num )
{
    char buf[1024] = {0};
    int ret = 0;
    while ( flg == 1 )
    {
        if ( arrayList.MsgRecv ( (void *)buf, -1) != 0 )
        {
            tolNum[num]++;
        }
    }
}

void outDo1 ( int num )
{
    char buf[1024] = {0};
    while ( flg == 1 )
    {
        if ( msgrcv ( msgID,buf,sizeof ( buf ), 0 , 0 ) > 0 )
        {
            tolNum[num]++;
        }
    }
}

void outDo2 ( int num )
{
    char buf[1024] = {0};
    while ( flg == 1 )
    {
        if ( msg_queue.MSGRcv ( buf, -1 ) == 1)
        {
            tolNum[num]++;
            //printf("Recv Msg\n");
        }
    }
}

void outDo3 ( int num )
{
    char buf[1024] = {0};
    while ( flg == 1 )
    {
        if ( queue.recv( buf, 1024, 0) >= 0)
        {
            tolNum[num]++;
            //printf("Recv Msg\n");
        }
    }
}



void inDo ( int index )
{
    int t = 20;
    char buf[1024] = {0};
    memset(buf,0x31,1024);
    while ( flg == 1 )
    {
        if ( arrayList.MsgSend ( buf ) == 0 ) 
        {
            tolNum[index]--;
        }
        std::this_thread::sleep_for ( std::chrono::microseconds( sleep_time ) );
    }
}

void inDo1 ( int index )
{
    int t = 20;
    char buf[1024] = {0};
    memset(buf,0x31,1024);
    while ( flg == 1 )
    {
        if ( msgsnd( msgID,buf,8,0 ) < 0  ) 
        {
            //tolNum[index]--;
        }
        std::this_thread::sleep_for ( std::chrono::microseconds( sleep_time ) );
    }
}



void inDo2 ( int index )
{
    int t = 20;
    char buf[1024] = {0};
    memset(buf,0x31,1024);
    while ( flg == 1 )
    {
        if (  msg_queue.MSGSnd ( buf ) < 0  ) 
        {
            printf("Send Failed\n");
            tolNum[index]--;
        }
        std::this_thread::sleep_for ( std::chrono::microseconds( sleep_time ) );
    }
}

void inDo3 ( int index )
{
    int t = 20;
    char buf[1024] = {0};
    memset(buf,0x31,1024);
    while ( flg == 1 )
    {
        if (queue.send(buf,1024,0) < 0  ) 
        {
            printf("Send Failed\n");
            tolNum[index]--;
        }
        std::this_thread::sleep_for ( std::chrono::microseconds( sleep_time ) );
    }
}




bool creat_msg_queue ()
{
    key_msg = ftok ( key_word, 'a' );
    msgID = msgget ( key_msg,IPC_CREAT|0666);
}

int main ( int argc, char * argv[] ) 
{
    int InThreadNum  = 4;
    int OutThreadNum = 8;
    int InInterval = 0;
    sleep_time = 1;
    long p = 1;
    long interval = 0;
    struct timeval v1;
    struct timeval v2;
    int zz = InThreadNum  > OutThreadNum ? InThreadNum : OutThreadNum;

    //creat_msg_queue ();
    queue.open(2, 1024, 1024);
    
    tolNum = (int *)malloc ( sizeof ( int ) * zz);
    memset ( tolNum,0x00,  sizeof ( int ) * zz);
    std::vector < std::thread > ThreadArray_out;
    std::vector < std::thread > ThreadArray_in;
    gettimeofday ( &v1, NULL);
    
    for ( int iLoop = 0; iLoop < OutThreadNum; iLoop++ )
    {
        ThreadArray_out.push_back ( std::thread ( outDo3,iLoop ) );
    }

    for ( int iLoop = 0; iLoop < InThreadNum; iLoop++ )
    {
        ThreadArray_in.push_back ( std::thread ( inDo3,iLoop ) );
    }
    
    while ( 1 )
    {
        sleep ( 2 );
        p = 1;
        for ( int iLoop = 0; iLoop < zz; iLoop ++ )
        {
            p += tolNum[iLoop];
            //tolNum[iLoop] = 0;
        }
        gettimeofday ( &v2, NULL);
        interval =  v2.tv_sec*1000000 + v2.tv_usec - v1.tv_sec*1000000 - v1.tv_usec;
        //interval = 1000;

        printf ("Times [%d] [%ld] TPS [%d] ART [%d]us\n", interval,p, p*1000  /  (interval / 1000), interval / p );
    }
    return 0; 
}
