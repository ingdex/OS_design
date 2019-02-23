#include <stdio.h>
//#include <>

#define BUFSIZE 10

int main(int argc , char *argv[]) {
    long int fSize; //the size of the readFile
    size_t bufSize;   //
    char buf[BUFSIZE];   //read buffer
    if (argc != 3)
    {
        printf("参数数量错误!\n");
        return -1;
    }
    FILE * readFile, * writeFile;
    if((readFile = fopen(argv[1], "rb")) == 0)
    {
        printf("readFile does not exsist\n");
        return -2;
    }
    if((writeFile = fopen(argv[2], "wb")) == 0)
    {
        printf("failed to creat writeFile\n");
        return -3;
    }
    fseek(readFile, 0, SEEK_END);
    fSize = ftell(readFile);
    rewind(readFile);
    while (fSize > 0)
    {
        bufSize = fread(buf, sizeof(char), BUFSIZE, readFile);
        fwrite(buf, sizeof(char), bufSize, writeFile);
        fSize = fSize - BUFSIZE;
    }
    return 0;
}