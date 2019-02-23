#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdlib.h>

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
    printf("writebuf started\n");
    if(argv != 5)
    {
        printf("writebuf参数错误\n");
        exit(1);
    }
    int SHMID = atoi(argc[1]);
    int SEMID = atoi(argc[2]);
    char * rfilename = argc[3];
    char * wfilename = argc[4];
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
    //printf("write file size:%d\n", fSize);
    if((pFile = fopen(wfilename, "wb")) == NULL)
    {
        printf("write文件打开失败\n");
        exit(2);
    }
    char buf[10];
    int j;
    //printf("write %d\n", fSize);
    while(fSize--)
    {
        P(SEMID, 0);
        P(SEMID, 2);
        //printf("in = %d, out = %d\n",shm->in, shm->out);
        for (j=0; j<shm->cnum[shm->out]; j++)
        {
            buf[j] = shm->buf[shm->out][j];
            //printf("%c", buf[j]);
        }
        fwrite(buf, 1, shm->cnum[shm->out], pFile);
        shm->out = (shm->out + 1) % 10;
        //printf("write: in = %d, out = %d\n", shm->in, shm->out);
        V(SEMID, 2);
        V(SEMID, 1);
    }
    fclose(pFile);
    printf("writebuf is over\n");
    return 0;
}
