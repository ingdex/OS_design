#ifndef PV_H
#define PV_H
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY1 0x11
#define KEY2 0x22
#define KEY  0x33
#define BUFSIZE 10

union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};

struct buffers {
    int count;
    char buf[BUFSIZE];
};

//P、V操作的函数定义：
void P(int semid, unsigned short index);
void V(int semid, unsigned short index);
#endif // PV_H
