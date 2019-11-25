#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

int main ()
{
    std::string d;
    std::string d1;

    char str[] = "123111111111111111111111111";
    char str1[16] = { 0 };
    int iLoop = 0;
    clock_t begin,end;

    begin = clock();
    while ( iLoop < 10000000 )
    {
        d.assign ( str, 10 );
        iLoop++;
    }
    end = clock ();
    std::cout << end - begin <<std::endl;
    iLoop = 0;
    begin = clock();
    while ( iLoop < 10000000 )
    {
        d = std::string ( str, 10 );
        iLoop++;
    }
    end = clock ();
    std::cout << end - begin <<std::endl;
    iLoop = 0;
    begin = clock();
    while ( iLoop < 10000000 )
    {
        memcpy ( str1, str, 10 );
        iLoop++;
    }
    end = clock ();
    std::cout << end - begin <<std::endl;

    iLoop = 0;
    begin = clock();
    while ( iLoop < 10000000 )
    {
        d1 = d;
        iLoop++;
    }
    end = clock ();

    std::cout << end - begin <<std::endl;
 
    

}
