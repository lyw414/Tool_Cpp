#include "Socket_Svr_Epoll.hpp"

void data_handle ( SOCKET sck, const std::string & data,void * UserData )
{
    return ;
}

int main () 
{
    LYW_CODE::Socket_Svr_Epoll epoll_svr( 1024,"Epoll_IPC" );
    epoll_svr.Start_Svr ( "0.0.0.0",2346,data_handle,NULL,1024,4 );
    sleep(2);
    epoll_svr.Stop_Svr ();
    return 0;
}

