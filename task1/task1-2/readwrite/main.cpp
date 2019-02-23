#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

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

//P、V操作的函数定义：
void P(int semid,int index);
void V(int semid,int index);

//信号灯、线程句柄定义：
int semid;
//子进程句柄定义
pid_t child1, child2;

int main(int argv, char * argc[])
{
    if(argv != 3)
    {
        printf("参数数量错误\n");
        return 0;
    }
    key_t key;
    key = ftok("/", 0);
    if(key < 0)
    {
        printf("ftok failed\n");
        return -1;
    }
    //创建共享内存组
    int shmid = shmget(key, sizeof(buffers), IPC_CREAT|0666);
    struct buffers * shm = (struct buffers *)shmat(shmid, NULL, SHM_R|SHM_W);
    shm->in = 0;
    shm->out = 0;
    //创建信号灯
    if ((semid = semget(key, 3, IPC_CREAT | 0666)) == -1)
    {
        printf("semget failed\n");
    }
    //信号灯赋初值；
    union semun arg;
    arg.val = 0;
    semctl(semid, 0, SETVAL, arg);  //满缓冲区数目
    arg.val = 10;
    semctl(semid, 1, SETVAL, arg);  //空缓冲区数目
    arg.val = 1;
    semctl(semid, 2, SETVAL, arg);  //缓冲区互斥操作信号灯
    //创建两个进程readbuf、writebuf
    //printf("shmid = %d, semid = %d, %d, %d, %d\n", shmid, semid, semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL), semctl(semid, 2, GETVAL));
    if((child1 = fork()) == 0)
    {   //sub process 1
        printf("Process 1 get created\n");
        if (execl("./readbuf", "./readbuf", to_string(shmid).c_str(), to_string(semid).c_str(), argc[1], NULL) < 0)
            printf("excl error\n");
        exit(0);
    }
    //child2 = fork();
    if((child2 = fork()) == 0)
    {   //sub process 2
        printf("Process 2 get created\n");
        if (execl("./writebuf", "./writebuf", to_string(shmid).c_str(), to_string(semid).c_str(), argc[1], argc[2], NULL) < 0)
            printf("excl error\n");
        exit(0);
    }
    //等待子进程1、2退出
    //wait(NULL);
    //wait(NULL);
    waitpid(child1, NULL, 0);
    printf("Child Process 1 is over\n");
    waitpid(child2, NULL, 0);printf("Child Process 2 is over\n");
    //删除信号灯
    semctl(semid, 0, IPC_RMID, arg);
    //删除共享内存
    shmctl(shmid,IPC_RMID,0);
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