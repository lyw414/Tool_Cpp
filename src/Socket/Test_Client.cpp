#include "Socket_Cli.hpp"
#include <thread>
#include <vector>
#include <stdlib.h>

std::string ip;
std::string port;
int  connect_num;
int thread_num;
int byte_num;
int interval;

void just_recv ( int index )
{
    return ;
}
void just_send ( int index )
{
    return ;
}

void do_client ( int index )
{
    int num;
    std::vector < LYW_CODE::Socket_Client > m_client_array;
    if ( index != thread_num - 1 )
    {
        num = connect_num / thread_num;
    }
    else
    {
        num = connect_num / thread_num;
        num = connect_num - num * index;
    }

    for ( int iLoop = 0; iLoop < num ; iLoop++  )
    {
        m_client_array.push_back ( LYW_CODE::Socket_Client );
        if ( m_client_array[iLoop].connect(ip,port) < 0 )
        {
            return -1;
        }
    }

    for ( int iLoop = 0; iLoop < num ; iLoop++  )
    {
        m_client_array[iLoop].send (
    }
    return ;
}

int main ( int argc, char * argv[] )
{
    ip = std::string ( argv[1] );
    port = atoi ( argv[2] );
    connect_num = atoi ( argv[3] );
    tread_num = atoi ( argv[4] );
    byte_num = atoi ( argv[5] );
    interval = atoi ( argv[6] );

    std::vector < std::thread > m_thread_array;

    for ( int iLoop = 0; iLoop < connect_num; iLoop++ )
    {
        m_thread_array.push_back ( std::thread ( do_client, iLoop ) );
    }

    while ( 1 )
    {
        std::this_thread::sleep ( std::chrono::seconds ( 2 ) );
    }
    
    for ( int iLoop = 0; iLoop < connect_num; iLoop++ )
    {
        m_thread_array[iLoop].join();
    }
    return 0;
}
