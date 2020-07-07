#include "FileStorage.hpp"
#include <stdio.h>

int main(int argc, char ** argv)
{
    LYW_CODE::FileStorage fileStorage;
    FHANDLE f;

    f.blockSize = 1024; 
    f.beginIndex = 0;
    int t = argc;

    t = 2;

    int flg = atoi(argv[1]);

    if (flg == 0)
    {
        f = fileStorage.falloc(1024);
        printf("Malloc [%ld]\n", f.beginIndex/1024);
    }
    else if (flg == 1)
    {
        int index = atoi(argv[2]);
        f.beginIndex = index * 1024;
        fileStorage.ffree(f);
    }

    //fileStorage.ffree(f);

    //f.beginIndex = 1024 * 1;
    //fileStorage.ffree(f);

    //f.beginIndex = 1024 * 2;
    //f.blockSize = 1024; 
    //fileStorage.ffree(f);

    //f.beginIndex = 1024 * 3;
    //fileStorage.ffree(f);


    //f = fileStorage.falloc(1024);
    //f = fileStorage.falloc(1024);
    //fileStorage.StorageData(f, (void *)"12345678", 8);


    //fileStorage.StorageData(f, (void *)"12345678", 8);

    //fileStorage.LoadData(f,buf,sizeof(buf));
    //printf ("Index [%ld] [%ld] %s\n",f.beginIndex, f.blockSize, buf);

    //fileStorage.ffree(f);
    fileStorage.SyncToIndexFile();

    return 0;
}
