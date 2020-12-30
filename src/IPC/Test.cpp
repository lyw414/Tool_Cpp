#include "MSGQueueForProc.hpp"
#include <stdio.h>
#include <unistd.h>

int main()
{
    char buf[1024] = {0};
    char buf1[1024] = {0};
    int iLoop = 0;
    LYW_CODE::MSGQueueForProc queue;
    queue.open(2, 1024, 1024);
    //sleep(10);
    //return 0;    
    while (iLoop < 20)
    {
        iLoop++;
        memset(buf1, 0x00, 1024);
        sprintf(buf,"%04d", iLoop);

        queue.send(buf,1024);
        printf("send:: %s \n\n", buf);

        memset(buf1, 0x00, 1024);
        queue.recv(buf1,1024);
        printf("Recv:: %s \n\n", buf1);
    }
    return 0;
}
