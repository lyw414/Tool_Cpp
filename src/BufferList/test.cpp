#include "BufferList.h"
#include <stdio.h>
#include <unistd.h>

#include <pthread.h>


pthread_mutex_t lock;

BufferList list;

void * push(void *)
{
    while (1)
    { 
        for (int iLoop = 0; iLoop < 15; iLoop++)
        {
            list.push_back((unsigned char *)&iLoop, sizeof(int));
            usleep(100000);
        }
    }

    return NULL;
}

void * show (void *)
{
    int out;
    while (1)
    { 
        if (list.pop_front((unsigned char *)&out) > 0)
        {
            printf ("%d\n", out);
        }
        else
        {
            usleep(500000);
        }
    }
    return NULL;
}

int main()
{
    void *status;
    list.Init(sizeof(int) * 20 + 1);
    pthread_t p;
    pthread_t p1;
    
    pthread_create(&p, NULL, push, (void *)NULL);
    pthread_create(&p1, NULL, show, (void *)NULL);

    pthread_join(p,&status);
    pthread_join(p1,&status);

    return 0;
}
