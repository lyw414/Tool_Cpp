#include "MSGQueueForProc.hpp"

int main(int argc, char * argv[])
{
    int  id = atoi(argv[1]);
    LYW_CODE::MSGQueueForProc queue;
    queue.open(id,1024,1024);
    queue.release();
    return 0;
}
