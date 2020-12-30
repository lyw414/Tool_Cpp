#include "MSGQueueForProc.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
    if (argc < 2) 
    {
        return 0;
    }
    int Num = atoi(argv[1]);
    int Interval = atoi(argv[2]);
    char buf[1024] = {0};
    memset(buf, 0x00, 1024);
    LYW_CODE::MSGQueueForProc queue;
    
    queue.open(2,1024,1024);
    
    while (Num > 0)
    {
        memset(buf, 0x00, 1024);
        sprintf(buf,"%04d",Num);
        queue.send(buf, 1024);
        printf("send ::  %s\n", buf);
        usleep(Interval);
        Num--;
    }

    return 0;
}

