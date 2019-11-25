#include "StrHashMap.hpp"
#include <unordered_map>
#include <map>
#include <time.h>
#include <stdio.h>
#include <list>
int main () 
{
    //StrHashMap smap ( 1000 );
    //LYW_CODE::JsonStrHashMap smap;
    LYW_CODE::JsonStrHashMap smap(1000);
    std::unordered_map < std::string, void * > pmap;
    std::map < std::string, void * > zmap;
    char  sKey[] = "1234567890123456";
    int len = 8;
    unsigned long iLoop = 0;
    std::string sp;
    std::string sp1;
    int findNun = 0;
    void * tmp;
    
    clock_t begin, end;
 

    begin = clock ();
    while ( iLoop < 10000000)
    {
        sKey[len - 1 - iLoop%4]++;
        smap.Find_Add ( sKey, len, &tmp);
        *(void**)tmp = sKey;
        iLoop++;
    }
    strcpy (sKey,"1234567890123456");
    iLoop = 0;
    while ( iLoop < 10000000)
    {
        sKey[len - 1 - iLoop%4]++;
        smap.Find ( sKey, len );
        iLoop++;
    }
    end = clock ();
    printf ( "time %d \n", end - begin );

    strcpy (sKey,"1234567890123456");
    iLoop = 0;
    begin = clock ();
    while ( iLoop < 10000000)
    {
        sKey[len - 1 - iLoop%3]++;
        sp1.assign(sKey, len );
        pmap[sp1] = sKey;
        iLoop++;
    }

    strcpy (sKey,"1234567890123456");
    iLoop = 0;
    while ( iLoop < 10000000)
    {
        sKey[len - 1 - iLoop%3]++;
        sp1.assign ( sKey, len );
        pmap.find(sp1);
        iLoop++;
    }
    end = clock ();
    printf ( "time %d \n", end - begin );

    iLoop = 0;
    strcpy (sKey,"1234567890123456");
    begin = clock ();
    while ( iLoop < 10000000)
    {
        sKey[len - 1 - iLoop%3]++;
        sp.assign ( sKey, len );
        zmap[sp] = sKey;
        iLoop++;
    }

    strcpy (sKey,"1234567890123456");
    iLoop = 0;
    while ( iLoop < 10000000)
    {
        sKey[len - 1 - iLoop%3]++;
        sp.assign ( sKey, len );
        zmap.find(sp);
        iLoop++;
    }
    end = clock ();
    printf ( "time %d \n", end - begin );

    iLoop = 0;
    strcpy (sKey,"1234567890123456");
    begin = clock ();
    while ( iLoop < 500000)
    {
        new LYW_CODE::JsonStrHashMap (10);
        iLoop++;
    }
    end = clock ();
    printf ( "time %d \n", end - begin );


    iLoop = 0;
    strcpy (sKey,"1234567890123456");
    begin = clock ();
    while ( iLoop < 500000)
    {
        new std::unordered_map < std::string, void * > ;
        iLoop++;
    }
    end = clock ();
    printf ( "time %d \n", end - begin );
 
    iLoop = 0;
    strcpy (sKey,"1234567890123456");
    begin = clock ();
    while ( iLoop < 500000)
    {
        std::map < std::string, void * > () ;
        iLoop++;
    }
    end = clock ();
    printf ( "time %d \n", end - begin );


    return 0;
}
