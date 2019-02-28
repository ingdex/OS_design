//
// Created by ingdex on 19-2-25.
//

#include "filesys.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

FileSys::FileSys(string filename)
{
    volumeName = filename;
    fstream volume(filename);
    if (!volume.is_open())
    {
        cout << "can't open file volume!" << endl;
        exit(0);
    }
    volume.read((char *)&managementBlock, sizeof(ManagementBlock));
    volume.read((char *)&inodeBitMap, sizeof(InodeBitMap));
    volume.seekg(BIT_MAP * BLOCK_SIEZ);
    volume.read((char *)&bitmap, sizeof(BitMap));
    volume.close();
}

FileSys::~FileSys()
{

}

int FileSys::init()
{
    fstream volume(volumeName);
    if (!volume.is_open())
    {
        cout << "open failed" << endl;
        exit(0);
    }
    char zero[BLOCK_SIEZ];
    memset(zero, 0, BLOCK_SIEZ);
    managementBlock = {S_ISIZE, S_FSIZE, S_NFREE, S_NINDOE-1};
    //初始化管理块
    volume.write(zero, BLOCK_SIEZ);
    volume.seekp(0, ios_base::beg);
    volume.write((char *)&managementBlock, sizeof(ManagementBlock));
    //初始化inode位示图
    inodeBitMap.byte[0] = static_cast<char>(0x80);
    for (int i=1; i<S_NINDOE/8; i++)
    {
        inodeBitMap.byte[i] = 0;
    }
    volume.write(inodeBitMap.byte, S_NINDOE/8);
    //初始化位示图块
    bitmap.byte[0] = static_cast<char>(0xff);  //0、1块、inode区已占用、第一个空闲块为存放主目录第一块
    bitmap.byte[1] = static_cast<char>(0xff);
    bitmap.byte[2] = static_cast<char>(0xfe);   //前23块已用
    for (int i=3; i<BLOCK_SIEZ; i++)
    {
        bitmap.byte[i] = 0;
    }
    volume.seekp(BLOCK_SIEZ, ios_base::beg);
    volume.write((char *)bitmap.byte, BLOCK_SIEZ);

    //debug
    volume.seekg(BLOCK_SIEZ);
    volume.read((char *)bitmap.byte, BLOCK_SIEZ);

    //初始化inode区
    volume.seekp(FIRST_INODE_BLOCK * BLOCK_SIEZ);
    for (int i=0; i<S_ISIZE; i++)
    {
        volume.write(zero, BLOCK_SIEZ);
    }

    Inode inode;
    inode.i_uid = -1;   //全部用户均可访问
    inode.i_type = DIR;   //fold
    inode.i_addr[0] = FIRST_FREE_BLOCK;
    for (int i=1; i<13; i++)
    {
        inode.i_addr[i] = 0;
    }
    volume.seekp(FIRST_INODE_BLOCK * BLOCK_SIEZ);
    volume.write((char *)&inode, sizeof(Inode));

    //debug
    volume.seekg(FIRST_INODE_BLOCK * BLOCK_SIEZ);
    volume.read((char *)&inode, sizeof(Inode));

    //主目录下目录项数置为2
    int entryCount = 2;
    volume.seekp(FIRST_FREE_BLOCK*BLOCK_SIEZ);
    volume.write((char *)&entryCount, sizeof(int));
    //写入./和../目录项
    Entry entry;
    entry.inodeNum = 0;
    strcpy(entry.name, "./");
    volume.write((char *)&entry, sizeof(Entry));
    entry.inodeNum = 0;
    strcpy(entry.name, "../");
    volume.write((char *)&entry, sizeof(Entry));

    //debug
    volume.seekg(FIRST_FREE_BLOCK*BLOCK_SIEZ, ios_base::beg);
    volume.read((char *)&entryCount, sizeof(int));

    volume.close();

    return 0;
}

int FileSys::createFile(string filename, char type, int uid, int parentInodeNum)
{
    fstream volume(volumeName);
    if (!volume.is_open())
    {
        cout << "open failed" << endl;
        exit(0);
    }
    //查找文件是否已存在
    Entry entry;
    int addr = 0, entryCount = 0;
    Inode parentInode = getInode(parentInodeNum);

    for (int i=0; i<10; i++)
    {
        if ((addr = parentInode.i_addr[i]) == 0)
        {   //文件不存在，进入下一步操作
            break;
        }
        volume.seekg(addr * BLOCK_SIEZ);
        volume.read((char *)&entryCount, sizeof(int));
        for (int j=0; j<entryCount; j++)
        {
            volume.read((char *)&entry, sizeof(Entry));
            if (strcmp(entry.name, filename.c_str()) == 0)
            {   //已有文件名为filename的文件
                Inode inode = getInode(entry.inodeNum);
                if (inode.i_type == REG_FILE)
                {
                    cout << "已经存在普通文件" << filename << endl;
                    return -1;
                }
                else if (inode.i_type == DIR)
                {
                    cout << "已经存在目录" << filename << endl;
                    return -1;
                }

            }
        }
        if (entryCount < MAX_ENTRY_COUNT)
        {   //不存在重名文件，继续下一步操作
            break;
        }

        //未考虑目录文件存在间接索引情况

    }
    //增加inode
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
    volume.write((char *)&inode, sizeof(Inode));

    //debug
    volume.seekg(FIRST_INODE_BLOCK*BLOCK_SIEZ + inodeNum*sizeof(Inode), ios_base::beg);
    volume.read((char *)&inode, sizeof(Inode));

    //初始化新文件i_addr[0]
    if (type == DIR)
    {   //如果为目录文件，写入./和../目录项
        int entryCount = 2;
        volume.seekp(inode.i_addr[0] * BLOCK_SIEZ);
        volume.write((char *)&entryCount, sizeof(int));
        Entry entry;
        entry.inodeNum = inodeNum;
        strcpy(entry.name, "./");
        volume.write((char *)&entry, sizeof(Entry));
        entry.inodeNum = parentInodeNum;
        strcpy(entry.name, "../");
        volume.write((char *)&entry, sizeof(Entry));
    }
    else
    {
        int charCount = 0;
        volume.seekp(inode.i_addr[0] * BLOCK_SIEZ);
        volume.write((char *)&charCount, sizeof(int));
    }

    //在父目录下建立目录项
    Inode inodeParent = getInode(parentInodeNum);
    //构建目录项
    entry.inodeNum = inodeNum;
    strcpy(entry.name, filename.c_str());
    //将目录项写入父目录块，不考虑需要增加间接索引情况
    //找到目录项要写入的块
    for (int i=0; i<10; i++)
    {
        addr = inodeParent.i_addr[i];
        if (addr == 0)
        {   //需要增加块
            addr = mallocBlock();
            if (addr == 0)
            {
                cout << "块数目不足" << endl;
                return -1;
            }

            inodeParent.i_addr[i] = addr;
            writeInode(inodeParent, parentInodeNum);
            entryCount = 0;
            volume.seekp(addr*BLOCK_SIEZ, ios_base::beg);
            volume.write((char *)&entryCount, sizeof(int));

            //debug
            volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
            volume.read((char *)&entryCount, sizeof(int));

            break;
        }
        //获取当前块中的目录项数目
        volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
        volume.read((char *)&entryCount, sizeof(int));

        if (entryCount < MAX_ENTRY_COUNT)
        {
            break;
        }
    }
    //写入目录项
    volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
    volume.read((char *)&entryCount, sizeof(int));
    entryCount++;
    volume.seekp(addr*BLOCK_SIEZ, ios_base::beg);
    volume.write((char *)&entryCount, sizeof(int));

    //debug
    volume.seekg(addr*BLOCK_SIEZ, ios_base::beg);
    volume.read((char *)&entryCount, sizeof(int));

    volume.seekp(addr*BLOCK_SIEZ + sizeof(int) + (entryCount-1)*sizeof(Entry), ios_base::beg);
    volume.write((char *)&entry, sizeof(Entry));

    //debug
    volume.seekg(addr*BLOCK_SIEZ + sizeof(int) + (entryCount-1)*sizeof(Entry), ios_base::beg);
    volume.read((char *)&entry, sizeof(Entry));

    volume.close();

    return inodeNum;
}

int FileSys::deleteFile(int inodeNum) {}

int FileSys::openFile(Inode inode) {}

int FileSys::closeFile(Inode inode) {}

int FileSys::readFile() {}

int FileSys::displayFile(int inodeNum)
{
    fstream volume(volumeName);
    Inode inode = getInode(inodeNum);
    int i = 0, j= 0, addr = 0;
    int entryCount = 0; //目录项数目
    int charCount = 0;  //字符数目
    Entry entry;
    string str; //文件内容
    char content[BLOCK_SIEZ];
    memset(content, 0, BLOCK_SIEZ);
    if (inode.i_type == DIR)  //是目录文件
    {
        for (i=0; i<10; i++)
        {
            if ((addr = inode.i_addr[i]) == 0)
            {
                break;
            }
            volume.seekg(addr*BLOCK_SIEZ);
            volume.read((char *)&entryCount, sizeof(int));
            for (int j=0; j<entryCount; j++)
            {
                volume.read((char *)&entry, sizeof(Entry));
                cout << entry.name;
                if (j == entryCount - 1)
                {
                    cout << endl;
                    break;
                }
                cout << " ";
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
            volume.read((char *)&charCount, sizeof(int));
            volume.read(content, charCount);
            content[charCount] = '\0';
            cout << content;
            if (charCount < MAX_CHAR_COUNT)
            {
                break;
            }

            //未考虑普通文件存在间接索引情况

        }
    }
    volume.close();

    return 0;
}

int FileSys::cdDir(int parentInodeNum, string dirName, int &curdir)          //进入目录
{
    int  i = 0;
    int addr = 0;
    fstream volume(volumeName);
    Entry entry;
    int entryCount = 0;
    Inode parentInode = getInode(parentInodeNum);
    for (i=0; i<10; i++)
    {
        if ((addr = parentInode.i_addr[i]) == 0)
        {
            cout << "目录不存在" << endl;
            return -1;
        }
        volume.seekg(addr*BLOCK_SIEZ);
        volume.read((char *)&entryCount, sizeof(int));
        for (int j=0; j<entryCount; j++)
        {
            volume.read((char *)&entry, sizeof(Entry));
            if (strcmp(entry.name, dirName.c_str()) == 0)
            {
                Inode inode = getInode(entry.inodeNum);
                if (inode.i_type != DIR)
                {
                    cout << dirName << "不是目录" << endl;
                    return -1;
                }
                curdir = entry.inodeNum;
                return 0;
            }
        }
        if (entryCount < MAX_ENTRY_COUNT)
        {
            cout << "目录不存在" << endl;
            return -1;
        }

        //未考虑目录文件存在间接索引情况

    }
    return -1;
}

int FileSys::catFile(int parentInodeNum, string filename)       //输出普通文件内容
{
    fstream volume(volumeName);
    Inode parentInode = getInode(parentInodeNum);
    Entry entry;
    int entryCount = 0, addr = 0, i = 0, inodeNum = 0;
    //找到父目录下的文件名为filename的文件，如果不存在输出错误信息
    for (i=0; i<10; i++)
    {
        if ((addr = parentInode.i_addr[i]) == 0)
        {   //文件不存在，创建文件
            cout << "不存在文件: " << filename << endl;
            return -1;
        }
        volume.seekg(addr * BLOCK_SIEZ);
        volume.read((char *)&entryCount, sizeof(int));
        for (int j=0; j<entryCount; j++)
        {
            volume.read((char *)&entry, sizeof(Entry));
            if (strcmp(entry.name, filename.c_str()) == 0)
            {   //找到文件名为filename的文件
                Inode inode = getInode(entry.inodeNum);
                if (inode.i_type != REG_FILE)
                {
                    cout << filename << "不是普通文件" << endl;
                    return -1;
                }
                else
                {
                    inodeNum = entry.inodeNum;
                    displayFile(inodeNum);
                    return 0;
                }

            }
        }
        if (entryCount < MAX_ENTRY_COUNT)
        {   //文件不存在
            cout << "不存在文件: " << filename << endl;
            return -1;
        }

        //未考虑目录文件存在间接索引情况

    }
}

int FileSys::writeFile(int parentInodeNum, string filename) //以更新的方式写文本文件
{
    fstream volume(volumeName);
    Inode parentInode = getInode(parentInodeNum);
    Entry entry;
    int entryCount = 0, addr = 0, i = 0, inodeNum = 0;
    //找到父目录下的文件名为filename的文件，如果不存在，则创建
    for (i=0; i<10; i++)
    {
        if ((addr = parentInode.i_addr[i]) == 0)
        {   //文件不存在，创建文件
            inodeNum = createFile(filename, REG_FILE, curUid, parentInodeNum);
            if (inodeNum == -1)
            {
                return -1;
            }
            break;
        }
        volume.seekg(addr*BLOCK_SIEZ);
        volume.read((char *)&entryCount, sizeof(int));
        int find = 0;
        for (int j=0; j<entryCount; j++)
        {
            volume.read((char *)&entry, sizeof(Entry));
            if (strcmp(entry.name, filename.c_str()) == 0)
            {   //已找到文件
                inodeNum = entry.inodeNum;
                find = 1;
                break;
            }
        }
        if (find)
        {
            break;
        }
        if (entryCount < MAX_ENTRY_COUNT)
        {   //文件不存在，创建文件
            inodeNum = createFile(filename, REG_FILE, curUid, parentInodeNum);
            if (inodeNum == -1)
            {
                return -1;
            }
            break;
        }

        //未考虑目录文件存在间接索引情况

    }
    //写文本文件，以更新的方式
    Inode inode = getInode(inodeNum);   //要写入的文件的inode
    if (inode.i_type != REG_FILE)
    {
        cout << filename << "不是普通文件" << endl;
        return -1;
    }
    char buf[512];
    int charCount = 0;
    char c;
    int addrNum = 0;
    addr = inode.i_addr[addrNum];   //要写入的文件的第一个存储块
    getchar();  //去掉命令中输入的换行符
    while ((c=getc(stdin)) != EOF)
    {
        //获取输入
        buf[charCount] = c;
        charCount++;
        //写入
        if (addr == 0)
        {   //文件已分配的存储块满，这里未考虑间接索引的情况
            addr = mallocBlock();
            if (addr == -1)
            {   //空闲块分配失败
                cout << "文件卷已满" << endl;
                return -1;
            }
            else
            {
                //更新inode区
                inode.i_addr[addrNum] = addr;
                writeInode(inode, inodeNum);
            }
        }
        if (charCount == MAX_CHAR_COUNT)    //输入的字符数达到一个空闲块可存储的最多字符数
        {
            volume.seekp(addr * BLOCK_SIEZ);
            volume.write((char *)&charCount, sizeof(int));
            volume.write(buf, MAX_CHAR_COUNT);
            //找到下一个存储块
            addrNum++;
            if (addrNum >= 10)
            {
                cout << endl << "超出文件大小范围" << endl;
                return -1;
            }
            charCount = 0;
            addr = inode.i_addr[addrNum];
        }
    }
    if (charCount == 0)
    {
        return 0;
    }
    volume.seekp(addr * BLOCK_SIEZ);
    volume.write((char *)&charCount, sizeof(int));
    volume.write(buf, charCount);

    //释放未用的存储块

    return 0;
}

int FileSys::rmFile(int parentInodeNum, string filename)
{
    
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

Inode FileSys::getInode(int inodeNum)
{
    fstream volume(volumeName);
    Inode inode;
    if (!volume.is_open())
    {
        cout << "open failed" << endl;
        exit(0);
    }

    volume.seekg(FIRST_INODE_BLOCK*BLOCK_SIEZ + inodeNum*sizeof(Inode), ios_base::beg);
    volume.read((char *)&inode, sizeof(Inode));
    volume.close();
    return inode;
}

int FileSys::writeInode(Inode inode, int inodeNum)
{
    fstream volume(volumeName);
    volume.seekp(FIRST_INODE_BLOCK*BLOCK_SIEZ + inodeNum* sizeof(Inode));
    volume.write((char *)&inode, sizeof(Inode));
    volume.close();
    return 0;
}

int FileSys::updateManegementBlock(ManagementBlock managementBlock)
{
    fstream volume(volumeName);
    //更新管理块
    volume.seekp(0, ios_base::beg);
    volume.write((char *)&managementBlock, sizeof(ManagementBlock));
    //更新inode位示图
    volume.write((char *)&inodeBitMap.byte, S_NINDOE/8);
    //更新位示图块
    volume.seekp(BLOCK_SIEZ, ios_base::beg);
    volume.write((char *)bitmap.byte, BLOCK_SIEZ);
    return 0;
}