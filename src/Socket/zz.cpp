#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main()
{
    struct sockaddr_in m_cli_addr;
    struct sockaddr_in m_svr_addr;
    int m_sck;
    int conn;
    unsigned int len ;
    char buffer[1024] = {0};
    m_sck = socket ( AF_INET, SOCK_STREAM , 0 );
    m_svr_addr.sin_family = AF_INET;
    m_svr_addr.sin_port = htons ( 12345 );
    m_svr_addr.sin_addr.s_addr = inet_addr ( "0.0.0.0");
    
     bind ( m_sck, ( struct sockaddr *) & m_svr_addr, sizeof ( m_svr_addr ) );
    listen ( m_sck, 5 );

    conn = accept ( m_sck, ( struct sockaddr * )&m_cli_addr, &len ) ;
    int ret = 0; 
    int total = 0;
    while ( 1 )
    {
        memset ( buffer, 0x00, sizeof ( buffer ) );
        ret = recv ( conn, buffer, sizeof ( buffer ), 0 );
        total += ret;
        printf ("recv %d\n",total);
    }
    return 0;


}
