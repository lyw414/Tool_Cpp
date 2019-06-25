#include <stdio.h>
#include <string.h>
#include "Msg_Queue_IPC.hpp"



int main ()
{
    int z = 0;
    void * p =  NULL; 
    void ** d = NULL;
    int s = 100;
    char bufs[1024] = {0};
    LYW_CODE:: Msg_Queue_IPC msg_queue ( "./IPC", 1024, 20 );
    
    msg_queue.MSGSnd ( bufs );
    
    msg_queue.MSGRcv ( bufs, 0);
    printf ("s's value[%s]\n",bufs);
    return 0;
}
