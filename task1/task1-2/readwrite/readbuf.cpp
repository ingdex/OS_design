#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string>

union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};

struct buffers {
    int in, out;
    char buf[10][10];
    int cnum[10];
};

void P(int semid,int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0; //操作标记：0或IPC_NOWAIT等
    semop(semid,&sem,1);	//1:表示执行命令的个数
    return;
}

void V(int semid,int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op =  1;
    sem.sem_flg = 0;
    semop(semid,&sem,1);
    return;
}

int main(int argv, char * argc[])
{
    printf("readbuf started\n");
    if(argv != 4)
    {
        printf("readbuf参数错误\n");
        exit(1);
    }
    int SHMID = atoi(argc[1]);
    int SEMID = atoi(argc[2]);
    char * rfilename = argc[3];
    //获取共享内存组
    struct buffers * shm = (struct buffers *)shmat(SHMID, NULL, SHM_R|SHM_W);
    FILE *pFile;
    if((pFile = fopen(rfilename, "rb")) == NULL)
    {
        printf("read文件打开失败\n");
        exit(1);
    }
    fseek(pFile, 0, SEEK_END);
    int fSize = ftell(pFile);
    if (fSize % 10 == 0)
    {
        fSize = fSize/10;
    } else
    {
        fSize = fSize/10 + 1;
    }
    rewind(pFile);
    char buf[10];
    int i = 0, j = 0;
    //printf("read %d\n", fSize);
    while(fSize--)
    {
        if ((i = fread(buf, 1, 10, pFile)) == 0)
            printf("fread error\n");
        P(SEMID, 1);
        P(SEMID, 2);
        //printf("%d, in = %d, out = %d\n", i, shm->in, shm->out);
        for (j=0; j<i; j++)
        {
            shm->buf[shm->in][j] = buf[j];
            //printf("%c", buf[j]);
        }
        shm->cnum[shm->in] = i;
        shm->in = (shm->in + 1) % 10;
        V(SEMID, 2);
        V(SEMID, 0);
    }
    fclose(pFile);
    printf("readbuf is over\n");
    return 0;
}
