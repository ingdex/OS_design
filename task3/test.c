#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(void)
{
    int dev, i = 0;
    char src[BUFSIZE];
    char des[BUFSIZE];
    printf("input the string you want to write to your device:\n");
    scanf("%s", src);
    if ((dev = open("/dev/mycdev", O_RDWR)) == -1)
    {
        printf("failed to open mycdev!\n");
	exit(0);
    }
    else
    {
        printf("success to open mycdev!\n");
    }
    printf("src string: %s\n", src);
    write(dev, src, BUFSIZE);
    lseek(dev, 2, SEEK_SET);
    read(dev, des, BUFSIZE);
    printf("des string: %s\n", des);
    close(dev);

    return 0;
}
