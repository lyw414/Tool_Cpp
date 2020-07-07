#include "Socket_Svr_Epoll.hpp"
#include <iostream>
unsigned long Msg;

void data_handle ( SOCKET sck, const char * data ,int len, void * UserData )
{
    __sync_fetch_and_add ( &Msg, len );
    //Msg += len;
    return ;
}

int main () 
{
    Msg = 0;
    int time = 0;
    long last = 0;
    LYW_CODE::Socket_Svr_Epoll epoll_svr( 1024,"Epoll_IPC" );
    epoll_svr.Start_Svr ( "127.0.0.1",12345,data_handle,NULL,10000,16 );
    while ( 1 )
    {
        if ( Msg == 0 )
        {
            sleep(1);
            continue;
        }

        sleep ( 2 );
        time+=2;
        std::cout << "Recv Msg Time ["<< time << "]s Total [" << Msg << "] TPS [" << ( Msg - last ) / 1024 / 2 << "]kb/s ART [" << time * 1000000 / Msg << "]us/byte" << std::endl;
        last = Msg;
    }
    epoll_svr.Stop_Svr ();
    return 0;
}

