#include <cstdio>
#include <string>
#include <iostream>
#include <stdarg.h>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include "Json.hpp"
#include "AnyValue.hpp"
using namespace LYW_CODE;

void getparam(int c, ...);
template < typename  ... Args >
void test(Args ... args)
{
    int s = sizeof...(args);
    std::tuple <Args...> p = std::make_tuple(args...);
    getparam(s, args...);
}
void getparam(int c, ...)
{
    int i = c;
    int sum = 0;
    void * p[10];

    va_list ap;
    va_start(ap, c);
    for (int iLoop = 0; iLoop < c; iLoop++)
    {
        p[iLoop] = va_arg(ap, char * );
    }
    va_end(ap);
    printf("%s %s %s\n", (char *)p[0], (char *)p[1], (char*)(p[2]));
}

std::vector < int > s()
{
    return { 1,2 };
}

int main()
{
    AnyValue a = 1;
    int s;
    s = a;
    printf("%d\n", s);
    return 0;
}
