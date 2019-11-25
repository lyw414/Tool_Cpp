#include "Json.hpp"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <string>
int main () 
{
    try 
    {
    LYW_CODE::Json js;
    const char * pp;
    std::string out3;
    std::string jsStr = "{ \"bool\" : 1.0,\"js\" : { \"1\" : 2, \"2\" : \"2\" }, \"array\": [{\"22\":2,\"dd\":3},1,\"2\"],  \"lyw_test\" \r : 1, \"lyw_test1\" : \"1231\" } ";
    //std::string jsStr;
    std::fstream f;
    f.open ( "./test.json",std::ios::in );
    getline ( f, jsStr );
    int len = 0;
    int iLoop = 0; 
    clock_t start,ends;
    start =clock();   

    LYW_CODE::Json json;

    pp = jsStr.c_str();
    len = jsStr.length();
    while ( iLoop < 10000)
    {

        json.parse ( (char *)pp,len);
        iLoop++;
    }
    ends =clock();   
    std::cout<<ends-start<<std::endl;
    //out3 = js.dump();
    //std::cout << out3 << std::endl;
    }
    catch ( std::exception & e )
    {
        std::cout << "Error :: " << e.what() << std::endl;
    }
    return 0;
}

