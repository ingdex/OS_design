//
// Created by ingdex on 19-2-25.
// inode[0]为主目录索引节点
// 最多支持32个用户，每个用户用id唯一标识，第i个用户的id第i位bit为1，其余位为0
// 目录文件的每一块用开始4字节记录本块存储的目录项个数
// 普通文件的每一块用开始4字节记录存放的字节数
#include <iostream>
#include <fstream>

using namespace std;

#ifndef FILESYS_FILESYS_H
#define FILESYS_FILESYS_H

#define BLOCK_SIEZ 512      //块大小为512B
#define S_ISIZE 20          //i结点区总块数
#define S_FSIZE 4096        //文件卷总块数, 4096 * 512B = 1MB
#define S_NFREE 490         //空闲块数
#define S_NINDOE 128        //空闲i结点数
#define FIRST_INODE_BLOCK 2 //第一块inode块位置
#define FIRST_FREE_BLOCK 22   //第一块空闲块位置
#define MAX_ENTRY_COUNT 15  //目录文件每一块可存放最大目录项数
#define MAX_CHAR_COUNT 508

struct ManagementBlock{   //管理块
    int s_isize;    //i结点区总块数
    int s_fsize;    //文件卷总块数
    int s_nfree;    //空闲块数
    int s_ninode;   //空闲i结点数
};

struct InodeBitMap{   //inode位示图
    char byte[S_NINDOE / 8];
};

struct BitMap{   //空闲块位示图
    char byte[BLOCK_SIEZ];
};

struct Inode{   //inode
    int i_uid;  //用户权限
    char i_type;     //文件类型，'0'：目录，'1'：普通文件
    int i_addr[13];   //地址索引表，实际只使用2级索引
};

struct user{
    char name[20];
    int id;
};

struct Entry{
    char name[28];
    int inodeNum;
};

class FileSys{
private:
    ManagementBlock managementBlock;
    InodeBitMap inodeBitMap;
    BitMap bitmap;
    Inode inode;
    string volumeName;
    int curOffset;
    int mallocInode();              //申请一个空闲inode，返回inode结点相对位置，不存在空闲inode时返回-1
    int mallocBlock();              //申请一个空闲块，返回空闲块绝对块号，不存在空闲块时返回-1
    int freeInode(int inodePos);    //释放inode
    int freeBlock(int blockPos);    //释放block
    Inode *getInode(int inodeNum);  //获取inode结点
    int writeInode(Inode const *inodep, const int inodeNum);
    int updateManegementBlock(ManagementBlock managementBlock);
public:
    FileSys(string filename);
    ~FileSys();

    int init();                     //初始化文件系统
    int createFile(string filename,char type, int uid, int parentInodeNum);    //建立文件
    int deleteFile(int inodeNum);    //删除文件
    int openFile(Inode inode);      //打开文件
    int closeFile(Inode inode);     //关闭文件
    int readFile();                 //读文件
    int writeFile();                //关闭文件
    int displayFile(int inodeNum);   //显示文件内容，如果是目录文件，显示目录项，如果是普通文件，显示文件内容

};

#endif //FILESYS_FILESYS_H
