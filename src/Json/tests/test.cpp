#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"
#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>

int main ( )
{
   std::string jsStr = "{ \"bool\" : false,\"js\" : { \"1\" : 2, \"2\" : \"2\" }, \"array\": [1,\"2\"],  \"lyw_test\" \r : 1, \"lyw_test1\" : \"1231\" } ";
   
   //std::vector < rapidjson::Document > json ( 10000 );
   rapidjson::Document p;
   std::fstream f;
   f.open ( "./test.json",std::ios::in );
   getline ( f, jsStr );
   //std::cout << jsStr << std::endl;
   int iLoop = 0;
   //rapidjson::Document json;
   clock_t start,ends;
   start =clock();
   while ( iLoop < 100000 )
   {
        p.Parse(jsStr.c_str());
        iLoop++;
   }
   ends = clock();
   std::cout<<ends-start<<std::endl;
   return 0;
}
