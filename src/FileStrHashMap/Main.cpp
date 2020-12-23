#include "FileStrHashMap.hpp"
#include <sys/time.h>
#include <string.h>


int total = 5000000;

int add( LYW_CODE::FileStrHashMap & m_map)
{
    char buf[1024];
    std::string key;

    struct timeval begin;
    struct timeval end;

    memset (buf, 0x31, sizeof(buf));
    gettimeofday(&begin, NULL);
    for (int iLoop = 0; iLoop < total; iLoop++)
    {
        std::string key = "Test_data:" + std::to_string(iLoop);
        m_map.add(key,buf,1024);
    }
    gettimeofday(&end, NULL);

    return ((end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec)) / total;
}


int find( LYW_CODE::FileStrHashMap & m_map)
{
    char buf[1024];
    std::string key;

    struct timeval begin;
    struct timeval end;

    memset (buf, 0x31, sizeof(buf));
    gettimeofday(&begin, NULL);
    for (int iLoop = 0; iLoop < total; iLoop++)
    {
        std::string key = "Test_data:" + std::to_string(iLoop);
        m_map.find(key,buf,1024);
    }
    gettimeofday(&end, NULL);

    return ((end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec)) / total;

}

int del( LYW_CODE::FileStrHashMap & m_map)
{
    char buf[1024];
    std::string key;

    struct timeval begin;
    struct timeval end;

    memset (buf, 0x31, sizeof(buf));
    gettimeofday(&begin, NULL);
    for (int iLoop = 0; iLoop < total; iLoop++)
    {
        std::string key = "Test_data:" + std::to_string(iLoop);
        m_map.del(key);
    }
    gettimeofday(&end, NULL);

    return ((end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec)) / total;


}

int main(int argc, char ** argv)
{
    LYW_CODE::FileStrHashMap m_map;
    m_map.Init(20);

    int t = 0;
    t = add (m_map);
    printf("add TPS[%d]\n", t);
    t = find (m_map);
    printf("add TPS[%d]\n", t);
    t = del (m_map);
    printf("add TPS[%d]\n", t);

}
