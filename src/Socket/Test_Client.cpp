#include "Socket_Cli.hpp"
#include <thread>
#include <vector>
#include <stdlib.h>
#include <string>
#include <iostream>

std::string ip;
int port;
int  connect_num;
int thread_num;
int byte_num;
int interval;
int * dataArray;

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
    int ret = 0;
    std::string str;
    char * buff = (char *)malloc ( byte_num );
    memset ( buff, 0x031, byte_num );
    str = std::string ( buff , byte_num );
    free ( buff );

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
        m_client_array.push_back ( LYW_CODE::Socket_Client() );
        while ( 1 )
        {
            ret = m_client_array[iLoop].connect(ip,port);
            std::this_thread::sleep_for ( std::chrono::milliseconds( 100 ) );
            if ( ret < 0 )
            {
                sleep ( 1 );
                printf ("Connect Failed [%d]\n",errno);
                m_client_array[iLoop].close();
                continue ;
            }
            else
            {
                break;
            }
        }
    }

    while ( 1 )
    {
        for ( int iLoop = 0; iLoop < num ; iLoop++  )
        {
            ret = m_client_array[iLoop].send ( str );
            if ( ret <= 0 )
            {
                std::cout << "Send Failed" << std::endl;
                m_client_array[iLoop].close();
                m_client_array[iLoop].connect(ip,port);

                std::this_thread::sleep_for ( std::chrono::milliseconds( 100 ) );
                continue;
            }
            dataArray[index] += byte_num;
            std::this_thread::sleep_for ( std::chrono::microseconds ( interval ) );
        }
    }
    return ;
}

int main ( int argc, char * argv[] )
{
    ip = std::string ( argv[1] );
    port = atoi ( argv[2] );
    connect_num = atoi ( argv[3] );
    thread_num = atoi ( argv[4] );
    byte_num = atoi ( argv[5] );
    interval = atoi ( argv[6] );
    int time = atoi ( argv[7] );
    long totalbyte = 0;
    dataArray = (int *) malloc ( thread_num * sizeof ( int ) );
    memset ( dataArray, 0x00, thread_num * sizeof ( int ) );

    std::vector < std::thread > m_thread_array;

    for ( int iLoop = 0; iLoop < thread_num; iLoop++ )
    {
        m_thread_array.push_back ( std::thread ( do_client, iLoop ) );
    }

    while ( time > 0 )
    {
        totalbyte = 0 ;
        for ( int iLoop = 0; iLoop < thread_num; iLoop++ )
        {
            totalbyte +=dataArray[iLoop];
        }
        printf ( "Send Msg left [%d]  [%ld]\n",time, totalbyte );
        std::this_thread::sleep_for ( std::chrono::seconds ( 2 ) );
        time = time -2;
    }
    
    for ( int iLoop = 0; iLoop < connect_num; iLoop++ )
    {
        m_thread_array[iLoop].join();
    }
    return 0;
}
