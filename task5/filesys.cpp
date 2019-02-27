#include "filesys.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

//
// Created by ingdex on 19-2-25.
//

FileSys::FileSys(string filename)
{
    volumeName = filename;
//    volume.open(filename);
//    if (!volume.is_open())
//    {
//        cout << "can't open file volume!" << endl;
//        exit(0);
//    }
//    int t = 1;
//    volume << t;
//    volume.seekg(0, ios_base::beg);
//    volume >> t;
//    cout << t;

}

FileSys::~FileSys()
{

}

int FileSys::init()
{
    fstream volume(volumeName);
    char zero[BLOCK_SIEZ];
    memset(zero, '0', BLOCK_SIEZ);
    managementBlock = {S_ISIZE, S_FSIZE, S_NFREE, S_NINDOE};
    //初始化管理块
    volume.write(zero, BLOCK_SIEZ);
    volume.seekp(0, ios_base::beg);
    volume << managementBlock.s_isize
         << managementBlock.s_fsize
         << managementBlock.s_nfree
         << managementBlock.s_ninode;
    //初始化inode位示图
    inodeBitMap.byte[0] = static_cast<char>(0x80);
    for (int i=1; i<S_NINDOE/8; i++)
    {
        inodeBitMap.byte[i] = 0;
    }
    volume << inodeBitMap.byte[0];
    for (int i=1; i<S_NINDOE/8; i++)
    {
        volume << inodeBitMap.byte[i];
    }
    //初始化位示图块
    bitmap.byte[0] = static_cast<char>(0xff);  //0、1块、inode区已占用、第一个空闲块为存放主目录第一块
    bitmap.byte[1] = static_cast<char>(0xff);
    bitmap.byte[2] = static_cast<char>(0xfe);
    for (int i=3; i<BLOCK_SIEZ; i++)
    {
        bitmap.byte[i] = 0;
    }
    volume.seekp(BLOCK_SIEZ, ios_base::beg);
    volume.write((char *)bitmap.byte, BLOCK_SIEZ);
    //初始化inode区
    for (int i=0; i<S_ISIZE; i++)
    {
        volume.write(zero, BLOCK_SIEZ);
    }
    volume.seekp(-S_ISIZE * BLOCK_SIEZ, ios_base::cur);
    inode.i_uid = -1;   //全部用户均可访问
    inode.i_type = '0';   //fold
    inode.i_addr[0] = FIRST_FREE_BLOCK;
    volume << inode.i_uid
        << inode.i_type
        << inode.i_addr[0];

    volume.close();
    return 0;
}

int FileSys::createFile(string filename, char type, int uid, int parentInodeNum)
{
    fstream volume(volumeName);
    int inodeNum = mallocInode();
    if (inodeNum <= 0)
    {
        cout << "i结点不足" << endl;
        return -1;
    }
    Inode inode;
    inode.i_type = type;
    inode.i_uid = uid;
    inode.i_addr[0] = mallocBlock();
    if (inode.i_addr[0] < 0)
    {
        freeInode(inodeNum);
        cout << "空闲块数不足" << endl;
        return -1;
    }
    for (int i=1; i<13; i++)
    {
        inode.i_addr[i] = 0;
    }
    volume.seekp(FIRST_INODE_BLOCK*BLOCK_SIEZ + inodeNum*sizeof(Inode), ios_base::beg);
    volume << inode.i_uid
        << inode.i_type
        << inode.i_addr[0];
    for (int i=1; i<13; i++)
    {
        volume << inode.i_addr[i];
    }

    //在父目录下建立目录项
    Inode *inodep = getInode(parentInodeNum);
    int addr = 0;
    int entryCount = 0;
    Entry entry;
    //构建目录项
    entry.inodeNum = inodeNum;
    strcpy(entry.name, filename.c_str());
    //将目录项写入父目录块，不考虑需要增加间接索引情况
    //找到目录项要写入的块
    for (int i=0; i<10; i++)
    {
        addr = inodep->i_addr[i];
        if (addr == 0)
        {   //需要增加块
            addr = mallocBlock();
            if (addr == 0)
            {
                cout << "块数目不足" << endl;
                return -1;
            }
            inodep->i_addr[i] = addr;
            volume.seekp(addr*BLOCK_SIEZ, ios_base::beg);
            entryCount = 0;
            volume << entryCount;
            break;
        }
        volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
        volume >> entryCount;
        if (entryCount < MAX_ENTRY_COUNT)
        {
            break;
        }
    }
    //写入目录项
    volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
    volume >> entryCount;
    entryCount++;
    volume.seekp(addr*BLOCK_SIEZ, ios_base::beg);
    volume << entryCount;
    volume.seekp(addr*BLOCK_SIEZ+(entryCount-1)*sizeof(Entry), ios_base::beg);
    volume.write((char *)&entry, sizeof(Entry));
    volume.close();

    return 0;
}

int FileSys::deleteFile(int inodeNum) {}

int FileSys::openFile(Inode inode) {}

int FileSys::closeFile(Inode inode) {}

int FileSys::readFile() {}

int FileSys::writeFile() {}

int FileSys::displayFile(int inodeNum)
{
    fstream volume(volumeName);
    Inode *inodep = getInode(inodeNum);
    int i = 0, addr = 0;
    int entryCount = 0; //目录项数目
    int charCount = 0;  //字符数目
    Entry entry;
    string str; //文件内容
    char content[BLOCK_SIEZ];
    memset(content, 0, BLOCK_SIEZ);
    if (inode.i_type == '0')  //是目录文件
    {
        for (i=0; i<10; i++)
        {
            if ((addr = inode.i_addr[i]) == 0)
            {
                break;
            }
            volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
            volume >> entryCount;
            for (int i=0; i<entryCount; i++)
            {
                volume.read((char *)&entry, sizeof(Entry));
                cout << entry.name << endl;
            }
            if (entryCount < MAX_ENTRY_COUNT)
            {
                break;
            }

            //未考虑目录文件存在间接索引情况

        }
    }
    else    //是普通文件
    {
        for (i=0; i<10; i++)
        {
            if ((addr = inode.i_addr[i]) == 0)
            {
                break;
            }
            volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
            volume >> charCount;
            volume.read(content, charCount);
            cout << content;
            if (charCount < MAX_CHAR_COUNT)
            {
                break;
            }

            //未考虑普通文件存在间接索引情况

        }
    }
    delete(inodep);
    volume.close();

    return 0;
}

int FileSys::mallocInode()
{
    int i;
    int j = 0;
    if (managementBlock.s_ninode <= 0)
    {
        return -1;
    }
    for (i=0; i<S_NINDOE/8; i++)
    {
        if (inodeBitMap.byte[i] == static_cast<char>(0xff))
        {
            continue;
        }
        if ((inodeBitMap.byte[i] & 0x80) == 0)
        {
            j = 0;
            inodeBitMap.byte[i] |= 0x80;
            break;
        }
        else if ((inodeBitMap.byte[i] & 0x40) == 0)
        {
            j = 1;
            inodeBitMap.byte[i] |= 0x40;
            break;
        }
        else if ((inodeBitMap.byte[i] & 0x20) == 0)
        {
            j = 2;
            inodeBitMap.byte[i] |= 0x20;
            break;
        }
        else if ((inodeBitMap.byte[i] & 0x10) == 0)
        {
            j = 3;
            inodeBitMap.byte[i] |= 0x10;
            break;
        }
        else if ((inodeBitMap.byte[i] & 0x08) == 0)
        {
            j = 4;
            inodeBitMap.byte[i] |= 0x08;
            break;
        }
        else if ((inodeBitMap.byte[i] & 0x04) == 0)
        {
            j = 5;
            inodeBitMap.byte[i] |= 0x04;
            break;
        }
        else if ((inodeBitMap.byte[i] & 0x02) == 0)
        {
            j = 6;
            inodeBitMap.byte[i] |= 0x02;
            break;
        }
        else if ((inodeBitMap.byte[i] & 0x01) == 0)
        {
            j = 7;
            inodeBitMap.byte[i] |= 0x01;
            break;
        }
    }
    if (i == S_NINDOE/8)
    {
        return -1;
    }
    managementBlock.s_ninode--;
    //更新管理块
    updateManegementBlock(managementBlock);

    return  i * 8 + j;
}

int FileSys::freeInode(int inodePos)
{
    if (inodePos == 0)  //属于第一级目录
    {
        return -1;
    }
    int i = inodePos / 8;
    int j = inodePos % 8;
    switch (j)
    {
        case 0:
            inodeBitMap.byte[i] &= 0x7f;
            break;
        case 1:
            inodeBitMap.byte[i] &= 0xbf;
            break;
        case 2:
            inodeBitMap.byte[i] &= 0xdf;
            break;
        case 3:
            inodeBitMap.byte[i] &= 0xe7;
            break;
        case 4:
            inodeBitMap.byte[i] &= 0xf7;
            break;
        case 5:
            inodeBitMap.byte[i] &= 0xfb;
            break;
        case 6:
            inodeBitMap.byte[i] &= 0xfd;
            break;
        case 7:
            inodeBitMap.byte[i] &= 0xfe;
            break;
        default:
            break;
    }
    managementBlock.s_ninode++;
    updateManegementBlock(managementBlock);

    return 0;
}

int FileSys::mallocBlock()
{
    int i;
    int j = 0;
    if (managementBlock.s_nfree <= 0)
    {
        return -1;
    }
    for (i=0; i<BLOCK_SIEZ/8; i++)
    {
        if (bitmap.byte[i] == static_cast<char>(0xff))
        {
            continue;
        }
        if ((bitmap.byte[i] & 0x80) == 0)
        {
            j = 0;
            bitmap.byte[i] |= 0x80;
            break;
        }
        else if ((bitmap.byte[i] & 0x40) == 0)
        {
            j = 1;
            bitmap.byte[i] |= 0x40;
            break;
        }
        else if ((bitmap.byte[i] & 0x20) == 0)
        {
            j = 2;
            bitmap.byte[i] |= 0x20;
            break;
        }
        else if ((bitmap.byte[i] & 0x10) == 0)
        {
            j = 3;
            bitmap.byte[i] |= 0x10;
            break;
        }
        else if ((bitmap.byte[i] & 0x08) == 0)
        {
            j = 4;
            bitmap.byte[i] |= 0x08;
            break;
        }
        else if ((bitmap.byte[i] & 0x04) == 0)
        {
            j = 5;
            bitmap.byte[i] |= 0x04;
            break;
        }
        else if ((bitmap.byte[i] & 0x02) == 0)
        {
            j = 6;
            bitmap.byte[i] |= 0x02;
            break;
        }
        else if ((bitmap.byte[i] & 0x01) == 0)
        {
            j = 7;
            bitmap.byte[i] |= 0x01;
            break;
        }
    }
    if (i == S_NINDOE/8)
    {
        return -1;
    }
    managementBlock.s_nfree--;
    updateManegementBlock(managementBlock);
    return  i * 8 + j;
}

int FileSys::freeBlock(int blockPos)
{
    if (blockPos == 0)  //属于第一级目录
    {
        return -1;
    }
    int i = blockPos / 8;
    int j = blockPos % 8;
    switch (j)
    {
        case 0:
            bitmap.byte[i] &= 0x7f;
            break;
        case 1:
            bitmap.byte[i] &= 0xbf;
            break;
        case 2:
            bitmap.byte[i] &= 0xdf;
            break;
        case 3:
            bitmap.byte[i] &= 0xe7;
            break;
        case 4:
            bitmap.byte[i] &= 0xf7;
            break;
        case 5:
            bitmap.byte[i] &= 0xfb;
            break;
        case 6:
            bitmap.byte[i] &= 0xfd;
            break;
        case 7:
            bitmap.byte[i] &= 0xfe;
            break;
        default:
            break;
    }
    managementBlock.s_nfree++;
    updateManegementBlock(managementBlock);
}

Inode * FileSys::getInode(int inodeNum)
{
    fstream volume(volumeName);
    Inode *inodep = new Inode;
    if (inodep == nullptr)
    {
        cout << "内存申请失败" <<endl;
        exit(0);
    }
    volume.seekg(FIRST_INODE_BLOCK*BLOCK_SIEZ+inodeNum* sizeof(Inode), ios_base::beg);
    volume.read((char *)inodep, sizeof(Inode));
    volume.close();
    return inodep;
}

int FileSys::writeInode(Inode const *inodep, const int inodeNum)
{
    fstream volume(volumeName);
    volume.seekp(FIRST_INODE_BLOCK*BLOCK_SIEZ+inodeNum* sizeof(Inode), ios_base::beg);
    volume.write((char *)inodep, sizeof(Inode));
    volume.close();
    return 0;
}

int FileSys::updateManegementBlock(ManagementBlock managementBlock)
{
    fstream volume(volumeName);
    volume.seekp(0, ios_base::beg);
    volume << managementBlock.s_isize
           << managementBlock.s_fsize
           << managementBlock.s_nfree
           << managementBlock.s_ninode;
    //更新inode位示图
    for (int i=0; i<S_NINDOE/8; i++)
    {
        volume << inodeBitMap.byte[i];
    }
    //更新位示图块
    volume.seekp(BLOCK_SIEZ, ios_base::beg);
    volume.write((char *)bitmap.byte, BLOCK_SIEZ);
    return 0;
}