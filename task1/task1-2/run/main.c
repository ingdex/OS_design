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
void P(int semid,int index);
void V(int semid,int index);

//信号灯、线程句柄定义：
int semid;
//子进程句柄定义
pid_t mget, mcopy, mput;

int * pcount1, * pcount2;

int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        printf("参数数量错误\n");
        return 0;
    }
    //创建共享内存组
    //buf1
    int shmid = shmget(KEY1, sizeof(struct buffers), IPC_CREAT|0666);
    struct buffers * shm1 = (struct buffers *)shmat(shmid, NULL, SHM_R|SHM_W);
    shm1->count = 0;
    //buf2
    shmid = shmget(KEY2, sizeof(struct buffers), IPC_CREAT|0666);
    struct buffers * shm2 = (struct buffers *)shmat(shmid, NULL, SHM_R|SHM_W);
    shm2->count = 0;
    //创建信号灯
    if ((semid = semget(KEY, 6, IPC_CREAT | 0666)) == -1)
    {
        printf("semget failed\n");
    }
    //信号灯赋初值；
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);  //缓冲区1是否为空
    arg.val = 0;
    semctl(semid, 1, SETVAL, arg);  //缓冲区1是否为满
    arg.val = 1;
    semctl(semid, 2, SETVAL, arg);  //缓冲区2是否为空
    arg.val = 0;
    semctl(semid, 3, SETVAL, arg);  //缓冲区2是否为满
    arg.val = 1;
    semctl(semid, 4, SETVAL, arg);  //缓冲区1读写互斥信号灯
    arg.val = 1;
    semctl(semid, 5, SETVAL, arg);  //缓冲区2读写互斥信号灯
	printf("in main semid = %d\n", semid);
    //创建进程
    if((mget = fork()) == 0)
    {   //sub process get
        printf("Process 1 get created\n");
        if (execl("./get", "./get", argv[1], argv[2], NULL) < 0)
            printf("excl error\n");
        exit(1);
    }
    if((mcopy = fork()) == 0)
    {   //sub process copy
        printf("Process 2 copy created\n");
        if (execl("./copy", "./copy", argv[1], argv[2], NULL) < 0)
            printf("excl error\n");
        exit(2);
    }
    if((mput = fork()) == 0)
    {   //sub process put
        printf("Process 3 put created\n");
        if (execl("./put", "./put", argv[1], argv[2], NULL) < 0)
            printf("excl error\n");
        exit(3);
    }
    //等待子进程退出
    waitpid(mget, NULL, 0);
    printf("get is over\n");
    waitpid(mcopy, NULL, 0);
    printf("copy is over\n");
    waitpid(mput, NULL, 0);
    printf("put is over\n");
    //删除信号灯
    semctl(semid, 0, IPC_RMID, arg);
    //删除共享内存
    shmctl(shmid, IPC_RMID, 0);
    return 0;
}

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
