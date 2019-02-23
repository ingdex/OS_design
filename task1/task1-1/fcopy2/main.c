#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define BUFSIZE 1024

void dirCopy(char *srcPath, char * desPath);
void fileCopy(char *srcPath, char * desPath);
int my_strcmp(const char *, const char *);
char * my_strcpy(char * s1, const char * s2);
char * my_strappend(char * s1, const char * s2);
void getFileOrDirName(char * filename, const char *path);

int main(int argc , char *argv[]) {
    size_t bufSize;   //
    char buf[BUFSIZE];   //read buffer
    struct stat fileStat;
    if (argc != 3)
    {
        printf("参数数量错误!\n");
        return -1;
    }
    stat(argv[1], &fileStat);
    if ((fileStat.st_mode & S_IFDIR) == S_IFDIR)
    {   //文件是目录文件
        printf("It's a directory\n");
        dirCopy(argv[1], argv[2]);
        return 0;
    }
    else{
        printf("It's a file\n");
        fileCopy(argv[1], argv[2]);
        return 0;
    }
}

void fileCopy(char *srcPath, char * desPath)
{
    size_t bufSize;   //
    char buf[BUFSIZE];   //read buffer
    char srcFilePath[4096], desFilePath[4096];
    char srcFilename[256], desFilename[256];
    my_strcpy(srcFilePath, srcPath);
    my_strcpy(desFilePath, desPath);
    struct stat fileStat;
    int readFile; //只读方式打开读文件
    int writeFile;
    readFile = open(srcPath, O_RDONLY); //只读方式打开读文件
    if (readFile == -1)
    {
        printf("failed to open source file\n");
        exit(0);
    }
    if(stat(desPath, &fileStat) == 0)
    {   //文件存在
        if((fileStat.st_mode & S_IFDIR) == S_IFDIR)
        {   //目标路径是目录，则复制产生目标文件路径为目标目录+原文件名
            getFileOrDirName(srcFilename, srcFilePath);
            my_strappend(desFilePath, "/");
            my_strappend(desFilePath, srcFilename);
        }
        //目标路径是文件
        writeFile = open(desFilePath, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);    //只写方式打开写文件
    }
    else    //目标文件不存在
    {
        writeFile = open(desFilePath, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);    //只写方式打开写文件
    }
    if (writeFile == -1)
    {
        printf("failed to open or create destination file\n");
        exit(1);
    }
    while ((bufSize = read(readFile, buf, BUFSIZE)) != 0)
    {
        write(writeFile, buf, bufSize);
    }
    close(readFile);
    close(writeFile);
}

void dirCopy(char *srcPath, char * desPath)
{
    DIR *srcDp;
    DIR *desDp;
    struct dirent *entry;
    struct stat statbuf;
    char srcFilePath[4096], desFilePath[4096];
    char srcFilename[256], desFilename[256];
    char tmpSrcFilePath[4096], tmpDesFilePath[4096];
    char desDirName[256];   //目标目录名
    my_strcpy(srcFilePath, srcPath);
    my_strcpy(desFilePath, desPath);
    srcDp = opendir(srcPath);
    if(srcDp == NULL)
    {
        printf("failed to open or create source dir\n");
        return;
    }
    if(stat(desFilePath, &statbuf) == 0)
    {   //目标路径存在
        if ((statbuf.st_mode & S_IFDIR) != S_IFDIR)
        {   //目标路径不是目录，错误
            printf("can't copy a dir to a file that is not a dir\n");
            exit(0);
        }
        //在目标路径下创建同名目录文件
        getFileOrDirName(srcFilename, srcFilePath);
        my_strappend(desFilePath, "/");
        my_strappend(desFilePath, srcFilename);
        if(mkdir(desFilePath, 0777) != 0)
        {
            printf("failed to create destination dir\n");
            exit(1);
        }
//        desDp = opendir(desPath);
    }
    else
    {   //目标路径不存在，即将源目录复制为一个带有新目录名的目录
        //创建desDir
        if(mkdir(desFilePath, 0777) != 0)
        {
            printf("failed to create destination dir\n");
            exit(2);
        }
        //desDp = opendir(desPath);
    }
//    if(desDp == NULL)
//    {
//        printf("failed to open destination dir\n");
//        exit(3);
//    }

    //将argv[1]，即readdir设置为当前目录;
    //chdir(argv[1]);
    while(entry = readdir(srcDp)){
        //以源目录项的名字为参数,调用lstat得到该目录项的相关信息;
        lstat(entry->d_name, &statbuf);
        if(entry->d_type == DT_DIR){  //statbuf.st_mode == S_IFDIR
            //目录项是目录文件
            if(my_strcmp(entry->d_name, "..") == 0 || my_strcmp(entry->d_name, ".") == 0)    //目录项的名字是”..”或”.”
            {
                continue;
            }
            //递归调用dirCopy()
            //构造源目录文件路径
            my_strcpy(tmpSrcFilePath, srcFilePath);
            my_strappend(tmpSrcFilePath, "/");
            my_strappend(tmpSrcFilePath, entry->d_name);
            dirCopy(tmpSrcFilePath, desFilePath);
        }
        else    //目录项不是目录文件
        {
            //构造源文件路径
            my_strcpy(tmpSrcFilePath, srcPath);
            my_strappend(tmpSrcFilePath, "/");
            my_strappend(tmpSrcFilePath, entry->d_name);
            //复制
            fileCopy(tmpSrcFilePath, desFilePath);
        }
    }
}

int my_strcmp(const char * str1, const char *str2)
{
    int i = 0;
    char c;
    while (c = str1[i])
    {
        if(c == str2[i])
        {
            i++;
            continue;
        }
        else
            return c>str2[i]?1:-1;
    }
    return str2[i]=='\0'?0:-1;
}

char * my_strcpy(char * s1, const char * s2)
{
    int i = 0;
    while (s1[i] = s2[i])
        i++;
    return s1;
}

char * my_strappend(char * s1, const char * s2)
{
    int i = 0, j = 0;
    while (s1[i] && i < 255)
        i++;
    while (s2[j] && i+j < 255)
    {
        s1[i+j] = s2[j];
        j++;
    }
    s1[i+j] = '\0';
    return s1;
}

void getFileOrDirName(char * filename, const char *path)
{
    char c;
    int i = 0;
    int filenameBegin = 0, filenameEnd = 0;
    while (c = path[filenameEnd])
    {
        if(c == '/')
        {
            //去除连续的'/'
            while (path[++filenameEnd] == '/');
            if(path[filenameEnd] == '\0')
            {   //文件路径以一个或者多个连续的'/'结尾
                while (i < filenameEnd - filenameBegin && i < 255)
                {
                    filename[i] = path[filenameBegin + i];
                    if(filename[i] == '/') break;
                    i++;
                }
                filename[i] = '\0';
                if (my_strcmp(filename, ".") == 0 || my_strcmp(filename, "..") == 0)
                {   //文件名是"."或者".."
                    filename[0] = '\0';
                }
                return;
            }
            else
            {
                filenameBegin = filenameEnd;
            }
        }
        filenameEnd++;
    }
    while (i < filenameEnd - filenameBegin + 1 && i < 255)
    {
        filename[i] = path[filenameBegin + i];
        i++;
    }
    filename[i] = '\0';
    if (my_strcmp(filename, ".") == 0 || my_strcmp(filename, "..") == 0)
    {   //文件名是"."或者".."
        filename[0] = '\0';
    }
}

//int main(int argc , char *argv[]) {
//    long int fSize; //the size of the readFile
//    size_t bufSize;   //
//    char buf[BUFSIZE];   //read buffer
//    if (argc != 3)
//    {
//        printf("参数数量错误!\n");
//        return -1;
//    }
//    FILE * readFile, * writeFile;
//    if((readFile = fopen(argv[1], "rb")) == 0)
//    {
//        printf("readFile does not exsist\n");
//        return -2;
//    }
//    if((writeFile = fopen(argv[2], "wb")) == 0)
//    {
//        printf("failed to creat writeFile\n");
//        return -3;
//    }
//    fseek(readFile, 0, SEEK_END);
//    fSize = ftell(readFile);
//    rewind(readFile);
//    while (fSize > 0)
//    {
//        bufSize = fread(buf, sizeof(char), BUFSIZE, readFile);
//        fwrite(buf, sizeof(char), bufSize, writeFile);
//        fSize = fSize - BUFSIZE;
//    }
//    return 0;
//}