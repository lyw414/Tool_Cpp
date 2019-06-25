#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
struct _msgsbuf 
{
        long mtype;
        char mtext[100];
};


int main ()
{
    
    key_t msg_key = 0;
    struct msqid_ds ds;

    struct _msgsbuf msg;
    struct _msgsbuf msgout1;

    memcpy ( msg.mtext,"123456",7);
    msg.mtype = 100;


    char buf[1024] = { 0 };
    msg_key = ftok ( "Test_LYW",'x');
    int msgID;
    msgID = msgget ( msg_key , IPC_CREAT|0666 );

    if ( msgID < 0 )
    {
        printf ( "create Msg Queue Failed!\n");
    }

    msgctl ( msgID,IPC_STAT,&ds );
    printf ( "[%d] \n",ds.msg_qbytes);


    long ss = 0;;

    while ( ss < 50000 )
    {
        msgsnd ( msgID, &ss,2, 0 );
        ss++;
    }

    msgrcv ( msgID, &msgout1, 0,0, 0 );
    printf ("recv ::%d %s\n",msgout1.mtype );


    return 0;
}

