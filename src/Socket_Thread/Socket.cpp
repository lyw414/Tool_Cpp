#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
std::string asc_to_bcd(std::string & asc)
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
 *          *组合 char 的高4位
 *                               */
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
 *          *组合 char 的低4位
 *                               */
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
 *          *获取高四位
 *                               */
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
 *          *获取低四位
 *                  */
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

int main(int argc, char *argv[])
{
    int sockfd = -1;
    //FILE * f;
    //f = fopen ( "./log.txt","a+");

    int res = -1;    
    char buf[1024] = {0};
    char bufx[1024] = {0};

    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( -1 == sockfd ) {
        perror( "sock created" );
        exit( -1 );
    }
    
    struct sockaddr_in server;    
    memset( &server, 0, sizeof( struct sockaddr_in ) );
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    //server.sin_addr.s_addr = inet_addr( "221.195.139.9" );
    server.sin_addr.s_addr = inet_addr( "192.168.1.1" );
    
    //std::string imsi1 = "607080900100000002010F00";
    time_t t;
    struct tm tt;
    int dd;
    std::string ss;
    std::string imsi;
    res = connect( sockfd, (struct sockaddr*)&server, sizeof( server ) );
    if( -1 == res )
    {
        perror( "sock connect" );
        exit( -1 );
    }

    printf ( "sock connect OK\n" );
    //std::string imsi1 = "60708090590100000102023FA38609000D393303CF1349E625BF4A3EBCF0E705B05D9336D7FA2F9A83878EF750EF65E777B513AB03000031353A35383A303500323031392F31312F313900";
    std::string imsi1 = "60708090590100000102023FA38609000D393303CF1349E625BF4A3EBCF0E705B05D9336D7FA2F9A83878EF750EF65E777B513AB030000";
    while ( 1 )
    {
        t = time(NULL);
        localtime_r (&t,&tt);
        std::string imsi = asc_to_bcd(imsi1);
        memset(bufx,0x00,1024) ;
        sprintf(bufx,"%02d:%02d:%02dx%04d/%02d/%02d\x00",tt.tm_hour,tt.tm_min,tt.tm_sec,tt.tm_year + 1900 ,tt.tm_mon + 1,tt.tm_mday);
        printf("%s\n",bufx);
        bufx[8] = 0x00;
        imsi += std::string(bufx,20);
        if ( send ( sockfd,imsi.c_str(),imsi.length(),0) < 0 )
        {
            printf ("send failed\n");
        }
        sleep(1);
        printf ("send [%d]\n",imsi.length());
        //dd = recv ( sockfd,buf,1024,0);
        //ss = bcd_to_asc(std::string (buf,dd));
        //printf ("recv [%s]\n",ss.c_str());
        //fwrite(ss.c_str(),1,ss.length(),f);
        //fwrite("\n",1,1,f);
    }
    //fclose(f);
    close ( sockfd );
    return 0;
}
