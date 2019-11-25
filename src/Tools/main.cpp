#include <cstdio>
#include "AnyValue.hpp"
#include "Log.hpp"
#include <string>
#include <iostream>
#include <stdarg.h>

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
int main()
{
    log::setLogMode(2);
    UserError("123131312");
    return 0;
}
