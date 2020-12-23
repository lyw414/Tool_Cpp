#include "FileStrHashMap.hpp"
#include <string.h> 
int main(int argc, char ** argv)
{
    LYW_CODE::FileStrHashMap m_map;
    m_map.Init(24);
    
    char buf[1024] = {0};
    if (argc < 2)
    {
        return 0;
    }

    if (strcmp(argv[1], "add") == 0)
    {
        m_map.add(std::string(argv[2]), (void *)argv[3], strlen(argv[3]));
        printf("Add Key [%s] value [%s]\n", argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "find") == 0)
    {
        m_map.find(std::string(argv[2]), buf, sizeof(buf));
        printf("Find Key [%s] value [%s]\n", argv[2], buf);
    }
    else if (strcmp(argv[1], "del") == 0)
    {
        m_map.del(std::string(argv[2]));
        printf("Delete Key [%s]\n", argv[2] );
    }

    return 0;
}
