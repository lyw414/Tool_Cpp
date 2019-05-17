#include "Log.hpp"
int main ( int argc, char * argv[] )
{
    Init_Log ( "/test/log/","LYW_Test",LYW_CODE::TIME_MODE,LYW_CODE::DEBUG_LOG,20,1024);
    UserError("in %s :: Test 1",__func__);
    UserError("in ssssssssssssssssssssssssssss dadaddddddddddddddddddddddddddddddddddddd");
    return 0;
}
