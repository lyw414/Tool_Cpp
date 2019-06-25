#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace LYW_CODE
{
    class Socket_Client
    {
    private:
        int m_conn;
        struct sockaddr_in m_addr;
        std::string m_ip;
        int m_port;
         
    public:
        Socket_Client ()
        {
            m_conn = -1;
        }

        ~Socket_Client ()
        {
            if ( m_conn > 0 )
            {
                close ( m_conn );
            }
        }

        int connect ( const std::string & ip, int port )
        {
            m_conn = socket(AF_INET,SOCK_STREAM,0);
            if ( m_conn < 0 )
            {
                return -1;
            }
            m_addr.sin_family = AF_INET;
            m_addr.sin_port = htons(port);
            m_addr.sin_addr.s_addr=inet_addr(ip.c_str());
            m_ip = ip;
            m_port = port;

            if ( connect ( m_conn,(struct sockaddr*)&their_addr,sizeof ( struct sockaddr ) ) < 0 )
            {
                close ( m_conn );
                m_conn = -1;
                return -1;
            }
            else
            {
                return 1;
            }
        }

        int recv ( std::string & Data, int maxlen )
        {
            int len = 0;
            Data = "";
            Data.resize ( maxlen );
            len = recv ( m_conn, (char *)Data.c_str(), maxlen,0 );
            if ( len == 0 )
            {
                close ( m_conn );
                m_conn = -1;
            }
            return len;
        }

        int send ( const std::string & Data )
        {
            int ret = 0;
            ret = send ( m_conn, Data.c_str(),Data.length(), 0 );
            return ret;
        }

        int close ( )
        {
            if ( m_conn > 0 )
            {
                close ( m_conn );
                m_conn = -1;
            }
            return 0;
        }
    }
}
