#include "FileStorage.hpp"


int main()
{
    LYW_CODE::FileStorage<> storage;
    printf("%d\n",sizeof(LYW_CODE::FileStorage<>::TIndexFileBlock));
    char buf[] = "12345678";
    char buf2[100] = {0};
    unsigned int p = storage.allocate(100);
    storage.write(p, buf, 8);
    int ret = storage.read(p,buf2,100);
    printf("%d %s\n",ret, buf2);
    storage.free(p);

    return 0;
}
