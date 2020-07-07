#include "FileStorage.hpp"


int main()
{
    LYW_CODE::FileStorage<> storage;
    char buf[] = "12345678";
    char buf2[100] = {0};
    void * p = storage.allocate(100);
    storage.write(p, buf, 8);
    storage.read(p,buf2,100);
    printf("%s\n", buf2);
    storage.free(p);

    return 0;
}
