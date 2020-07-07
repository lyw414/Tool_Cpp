#include <string>
#include <thread>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mutex>

#include "Msg_Queue_IPC.hpp"
#include <unordered_map>
#ifdef  WIN32
#define SOCKET int
#define CALLBACK __stdcall
#endif 

#ifdef __linux__
#define SOCKET int 
#define CALLBACK  
#endif


namespace LYW_CODE
{
    typedef void ( * TDataHandle_Func_Call_Back ) ( SOCKET sck, const char * data, int len, void * UserData );

    typedef enum _Sokcet_State
    {
        Connect_Failed_Socket,
        Connect_Socket,
        DisConnect_Socket
    } TSocket_State;

    typedef struct _client_info
    {
        std::string ip;
        SOCKET sck;
        int port;
    } TClient_Info;
    

    class Socket_Svr_Epoll
    {
    private :
        std::string m_ip;
        int m_port;
        SOCKET m_sck;
        size_t m_max_connect_num;
        size_t m_thread_num;
        struct sockaddr_in m_cli_addr;
        struct sockaddr_in m_svr_addr;
        std::unordered_map < SOCKET, TClient_Info > m_cli_info;
        std::mutex m_sck_info_lock;
        int m_epfd;
        std::vector < std::thread > m_work_thread_array;
        std::thread m_control_thread;
        struct epoll_event * m_epoll_events;
        int m_svr_status;
        Msg_Queue_IPC m_event_msg_queue;
        //user data handle function call back, if not null , recv will be unusefull
        TDataHandle_Func_Call_Back m_handle_func;
        void * m_UserArgs;

        //entry of work thread 
        static void work_thread ( void * p )
        {
            Socket_Svr_Epoll * svr = ( Socket_Svr_Epoll *) p;
            svr -> work_thread_run();
        }
        
        //blocking event msg queue with timeout handle msg
        void work_thread_run ()
        {
            //need finish it
            SOCKET sck;
            char buffer [4096] = {0};
            int ret = 0;
            struct epoll_event ev = { 0 };


            while ( m_svr_status == 1 )
            {
                ret = m_event_msg_queue.MSGRcv ( &sck, 1 );
                if ( ret == 1 )
                {
                    //get MSG from msg queue 
                    while ( 1 )
                    {
                        //recv msg
                        ret = recv ( sck, buffer, sizeof ( buffer ), 0 );
                        

                        //recv err
                        if ( ret < 0 )
                        {
                            //recv error 
                            if ( ret < 0  && ( errno == EWOULDBLOCK || errno == EAGAIN ) )
                            {
                                //no recv data buf sck is ok
                                //printf ( "finish read \n");
                                break;

                            }
                            else if ( errno == EINTR )
                            {
                                printf ( "need read again\n");
                                continue;
                            }
                            else
                            {
                                //other error close sck
                                close( sck );
                                epoll_ctl ( m_epfd, EPOLL_CTL_DEL, sck, &ev );
                                break;

                            }

                        }

                        //recv close 
                        if ( ret == 0 )
                        {
                            //recv disconnect
                            //colse and delete sck fron epoll
                            close ( sck ) ;
                            epoll_ctl ( m_epfd, EPOLL_CTL_DEL, sck, &ev );
                        }

                        //recv data
                        if ( ret < (int)sizeof ( buffer ) )
                        {
                            //continue recv data
                            if (  m_handle_func != NULL )
                            {
                                m_handle_func ( sck, buffer, ret,m_UserArgs );
                            }
                            break;
                        }
                        else
                        {
                            //recieve all data 
                            if (  m_handle_func != NULL )
                            {
                                m_handle_func ( sck, buffer, ret, m_UserArgs );
                            }
                            continue;
                        }
                    } // end while finish recv 

                    //callback handle this msg
                    //if ( msg.length() > 0 && m_handle_func != NULL )
                    //{
                    //    m_handle_func ( sck, msg, m_UserArgs );
                    //}
                }
            }
        }

        //entry of control thread 
        static void control_thread ( void * p )
        {
            Socket_Svr_Epoll * svr = (Socket_Svr_Epoll *) p;
            svr -> control_thread_run ();
        }

        //blocking epoll handlde and throws events to evenet msg queue
        void control_thread_run () 
        {
            int nfds = 0;
            int conn = 0;
            unsigned int len;
            struct epoll_event ev;
            int retry = 0;
            int connect_num = 0;
            while ( m_svr_status == 1 )
            {
                
                if ( ( nfds = epoll_wait( m_epfd, m_epoll_events, m_max_connect_num, 1000 ) ) <= 0 )
                {
                    //timeout or other error, check svr status
                    continue;

                }

                //travel the event handle
                for ( int iLoop = 0; iLoop < nfds; iLoop++ )
                {
                    // accept connect handle it in control thread 
                    if ( m_epoll_events[iLoop].data.fd == m_sck )
                    {
                        retry = 0;
                        while ( retry <= 3 )
                        {
                            memset ( &m_cli_addr, 0x00, sizeof ( struct sockaddr ) );
                            //accept connect add regist connect handle to epoll
                            if ( ( conn = accept ( m_sck, ( struct sockaddr * )&m_cli_addr, &len ) ) < 0 )
                            {
                                //accept error 
                                //printf ("accept Error![%d]\n", errno);
                                retry++;
                                continue;
                            }
                            else
                            {
                                connect_num++;
                                printf ( "Connect NUM [%d]\n", connect_num );
                                //set no blocking 
                                fcntl(conn, F_SETFL, fcntl(conn, F_GETFL, 0) | O_NONBLOCK);
                                //regist it 
                                ev.data.fd = conn;
                                ev.events = EPOLLIN | EPOLLET;
                                //epoll_ctl( m_epfd, EPOLL_CTL_ADD, conn, &ev );
                            }     
                        }
                    }
                    else if ( m_epoll_events[iLoop].events & EPOLLIN )
                    {
                        m_event_msg_queue.MSGSnd ( & ( m_epoll_events[iLoop].data.fd ) );
                    }
                }
            }
        }

    public:
        Socket_Svr_Epoll ( size_t epoll_ipc_size, const std::string & epoll_key ) : m_event_msg_queue ( epoll_key,sizeof ( SOCKET ), epoll_ipc_size )
        {
            m_ip = "";
            m_port = -1;
            m_max_connect_num = 4096;
            m_svr_status = 0;
            m_handle_func = NULL;
            m_UserArgs = NULL;
        }

        int Start_Svr ( const std::string & ip, const int port, TDataHandle_Func_Call_Back func = NULL, void * UserData = NULL , const size_t max_connect_num = 4096, const size_t thread_num = 4)
        {
            int ret = 0;
            if ( m_ip == "" )
            {
                m_ip = "0.0.0.0";
            }
            else
            {
                m_ip = ip;
            }
            //set data handle call back 
            m_handle_func = func;
            m_UserArgs = UserData;

            m_port = port;
            m_max_connect_num = max_connect_num;
            m_thread_num = thread_num;
            int reuse = 1;
            // no blocking 
            m_sck = socket ( AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0 );
            if ( m_sck < 0 )
            {
                return -1;
            }
            // set m_svr_addr
            m_svr_addr.sin_family = AF_INET;
            m_svr_addr.sin_port = htons ( m_port );
            m_svr_addr.sin_addr.s_addr = inet_addr ( m_ip.c_str() );

            //set reuse, in case bind faild
            setsockopt ( m_sck, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof ( reuse ) );
            //bind 
            ret = bind ( m_sck, ( struct sockaddr *) & m_svr_addr, sizeof ( m_svr_addr ) ) ;
            if ( ret < 0 )
            {
                return ret;
            }
            
            //listen
            ret = listen ( m_sck, 5 );
            if ( ret < 0 )
            {
                return ret;
            }

            //allocate buffer for epoll_event
            m_epoll_events = new struct epoll_event [ m_max_connect_num ];

            //init epoll
            m_epfd = epoll_create ( m_max_connect_num );

            //regist listen socket to epoll 
            struct epoll_event ev = { 0 };
            ev.data.fd = m_sck;
            ev.events = EPOLLIN | EPOLLET;
            epoll_ctl ( m_epfd, EPOLL_CTL_ADD, m_sck, &ev );

            m_svr_status = 1;

            //creat control thread to listen epoll handle and put events to event msg queue
            m_control_thread = std::thread ( control_thread, this );
            
            //creat work thread 
            for ( size_t iLoop = 0; iLoop < m_thread_num; iLoop++ )
            {
                m_work_thread_array.push_back ( std::thread ( work_thread, this ) );
            }
            return 0;
        }

        int Stop_Svr ( )
        {
            m_svr_status = 0;
            //join all thread 
            
            m_control_thread.join();
            for ( size_t iLoop = 0; iLoop < m_work_thread_array.size(); iLoop++)
            {
                m_work_thread_array[iLoop].join();
            }
            delete m_epoll_events;
            close ( m_epfd );
            return 0;
        }

        int disconnect ( SOCKET sck )
        {
            struct epoll_event ev = { 0 };
            //EPOLL unregist sck 
            close ( sck ) ;
            epoll_ctl ( m_epfd, EPOLL_CTL_DEL, sck, &ev );
            return 0;
        }

        int Recv ( SOCKET & sck, std::string & outStr );
        int Send ( const SOCKET & sck, const std::string & inStr );
        int GetConnectInfo ( const SOCKET & sck );
    };
}
