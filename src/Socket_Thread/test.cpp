#include <stdio.h>
#include <iostream>
#include <chrono>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

std::string asc_to_bcd(std::string asc)
{
    std::string bcd;
    int strlen = asc.length();
    unsigned char bcd_char = 0;
    unsigned char * szAscStr = (unsigned char *)asc.c_str();

    if (strlen % 2 != 0)
    {
        return bcd;
    }

    /*检查 字符 若为非 0 ~ F 则直接报错出去*/
    for (int iLoop = 0; iLoop < strlen / 2; iLoop++)
    {
        /*
         *组合 char 的高4位
                     */
        if (szAscStr[2 * iLoop] >= '0' && szAscStr[2*iLoop] <= '9')
        {
            bcd_char = ((unsigned char)(szAscStr[2*iLoop] - '0')) << 4;
        }
        else if (szAscStr[2*iLoop] >= 'A' && szAscStr[2*iLoop] <= 'F')
        {
            bcd_char = ((unsigned char)(szAscStr[2*iLoop] - 'A' + 10)) << 4;
        }
        else
        {
            bcd = "";
            return bcd;
        }

        /*
         *组合 char 的低4位
                     */
        if (szAscStr[2* iLoop + 1] >= '0' && szAscStr[2*iLoop + 1] <= '9')
        {
            bcd_char = (bcd_char & 0xF0) + (szAscStr[2*iLoop + 1] - '0');
        }
        else if (szAscStr[2*iLoop + 1] >= 'A' && szAscStr[2*iLoop + 1] <= 'F')
        {
            bcd_char = (bcd_char & 0xF0) + (szAscStr[2*iLoop + 1] - 'A' + 10);
        }
        else
        {
            bcd = "";
            return bcd;
        }
        bcd += bcd_char;
    }
    return bcd;
}

std::string bcd_to_asc(std::string bcd)
{
    std::string asc;
    unsigned char asc_char;
    unsigned char * sBcdStr = (unsigned char *)bcd.c_str();
    int strlen = bcd.length();
    for (int iLoop = 0; iLoop < strlen; iLoop++)
    {
        /*
         *获取高四位
                     */
        asc_char = ((sBcdStr[iLoop] & 0xF0) >> 4) & 0x0F;
        if (asc_char <= 0x09)
        {
            asc += asc_char + '0';
        }
        else
        {
            asc += asc_char + 'A' - 10;
        }

        /*
         *获取低四位
        */
        asc_char = sBcdStr[iLoop] & 0x0F;
        if (asc_char <= 0x09)
        {
            asc += asc_char + '0';
        }
        else
        {
            asc += asc_char + 'A' - 10;
        }
    }
    return asc;
}



bool generate_heart_beat_response_msg(int ID, std::string & msg)
{
    tm now;
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    now = *localtime(&t);
    unsigned char pDownData[256] = { 0 };
    memset(pDownData, 0x00, 256);
    int nIdex = 0;

    pDownData[nIdex++] = 0x60;
    pDownData[nIdex++] = 0x70;
    pDownData[nIdex++] = 0x80;
    pDownData[nIdex++] = 0x90;

    /////////TerminalID//////////////////////////
    memcpy(pDownData + nIdex, &ID, 4);
    nIdex += 4;
    pDownData[nIdex++] = 0x02;
    pDownData[nIdex++] = 0x01;
    pDownData[nIdex++] = 0x0F;
    //消息长度
    pDownData[nIdex++] = 30;
    //消息内容
    pDownData[nIdex++] = 0; //用户信息序号 4BYTE
    pDownData[nIdex++] = 0;
    pDownData[nIdex++] = 0;
    pDownData[nIdex++] = 0;
    pDownData[nIdex++] = 1; //消息类型 1BYTE
    pDownData[nIdex++] = 24;//参数长度 1BYTE
    pDownData[nIdex++] = (unsigned char)(now.tm_year + 1990);
    pDownData[nIdex++] = (unsigned char)((now.tm_year + 1990) >> 8);
    pDownData[nIdex++] = (unsigned char)(now.tm_mon + 1);
    pDownData[nIdex++] = (unsigned char)(now.tm_mday);
    pDownData[nIdex++] = (unsigned char)(now.tm_hour);
    pDownData[nIdex++] = (unsigned char)(now.tm_min);
    pDownData[nIdex++] = (unsigned char)(now.tm_sec);
    memcpy(pDownData + nIdex, "admin", 5);
    nIdex += 16;
    pDownData[nIdex++] = 1; //加密类型 1BYTE 1加密 0不加密
    int nDataLen = nIdex;
    msg = std::string((char *)pDownData, nDataLen);
    return true;
}

int main()
{

    struct sockaddr_in server;
    int sck;
    int acceptfd;
    int ret;
    char buf[1024] = {0};
    std::string h;
    std::string Ah;
    std::string Bh;


    std::string Ah1;
    std::string Bh1;

    std::string Ah2;
    std::string Bh2;

    std::string Ah3;
    std::string Bh3;

    generate_heart_beat_response_msg(35,h);
    //Ah = "60708090230000000201890600000000040400000800";
    Ah = "607080902300000002018806000000001300";
    Bh = asc_to_bcd(Ah);


    Ah3 = "607080902300000002018806000000001000";
    Bh3 = asc_to_bcd(Ah3);


    //Ah2 = "60708090 23000000 020188 
    //       len
    //       01000000
    //       13
    //       len
    //       01
    //       len 12
    //       01
    //       01
    //       31313131
    //       31313131
    //       31313131
    //       31313100";
    
    Ah2 = "607080902300000002018818000000001112020132323231313131313131313131313100";
    // Ah2 = "6070809023000000020188290100000011230201303030303232323231313131313131000131313131313131313131313131313100";
 
    Bh2 = asc_to_bcd(Ah2);
    //printf ("%d %d\n",Ah.length(),Bh.length() );

    sck = socket ( AF_INET, SOCK_STREAM, 0 );
    memset( &server, 0, sizeof( struct sockaddr_in ) );
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = inet_addr( "0.0.0.0" );

    int bindres = bind( sck, (struct sockaddr*)&server, sizeof( server ) );
    if( -1 == bindres ) 
    {
        printf ( "sock bind\n" );
        return 0;
    }
    
    int listenres = listen( sck, SOMAXCONN );
    if( -1 == listenres ) 
    {
        printf ( "sock listen\n" );
        return 0;
    }
    
    struct sockaddr_in peerServer;
    socklen_t len = sizeof( peerServer );
    
    struct timeval timeout = {2,0};
    if ( setsockopt(sck, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) != 0 )
    {
        printf ( "setsocket opt error!\n" );
        return 0;
    }
    
    while ( 1 )
    {
        acceptfd = accept( sck, (struct sockaddr*)&peerServer, &len );
        if ( acceptfd < 0 )
        {
            //printf ( "accept timeout!\n" );
            continue;
        }
        else 
        {
            break;
        }
    }

    printf ("accept OK!\n");
    timeout.tv_sec = 1;
    if ( setsockopt(acceptfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) != 0 )
    {
        printf ( "setsocket opt error!\n" );
        return 0;
    }
 

    send ( acceptfd,Bh2.c_str(),Bh2.length(),0);
    //send ( acceptfd,Bh3.c_str(),Bh3.length(),0);
    while ( 1 )
    {
        memset ( buf,0x00,1024);
        ret = recv ( acceptfd, buf,1024, 0 );
        if ( ret < 0 )
        {
            printf ("recv timeout\n");
        }
        else if ( ret > 0 )
        {
            Ah1 = bcd_to_asc(std::string (buf,ret));
            generate_heart_beat_response_msg(35,h);
            printf ("%s\n",Ah1.c_str());
            send ( acceptfd,Bh.c_str(),Bh.length(),0);
            send ( acceptfd,h.c_str(),h.length(),0);
            sleep(1);
        }
        else
        {
            sleep(2);
            close ( acceptfd );
            close ( sck );
            break;
        }

    }

    return 0;
}
