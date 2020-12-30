#include "MSGQueueForProc.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
    if (argc < 1) 
    {
        return 0;
    }
    int Num = atoi(argv[1]);
    char buf[1024] = {0};
    LYW_CODE::MSGQueueForProc queue;
    
    queue.open(2,1024,1024);
    
    while (Num > 0)
    {
        memset(buf, 0x00, 1024);
        queue.recv(buf, 1024, 1);
        printf("recv :: %s\n", buf);
        Num--;
    }

    return 0;
}

