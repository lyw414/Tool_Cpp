#include "File_BST.hpp"
#include <stdio.h>

int main()
{
    LYW_CODE::File_ f;
    std::string OutStr;
    f.open ( "./test" );
    f.write ( (const unsigned char *)"1111", 4 );
    f.write ( (const unsigned char *)"2222", 4 );
    f.seek ( 0 );
    f.read ( OutStr, 8 );
    printf ( "%s\n", OutStr.c_str() );
    return 0;
}
