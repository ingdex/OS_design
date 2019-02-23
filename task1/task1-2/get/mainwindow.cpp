#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pv.h"
#include <stdio.h>
#include <QTimer>

QTimer * timer;
long int i;
int semid;
int shmid;
FILE * srcFile;
FILE * desFile;
struct buffers * shm1;
struct buffers * shm2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("get");
    //获取共享内存组
    int shmid;
    if ((shmid = shmget(KEY1, sizeof(buffers), IPC_CREAT|0666)) == -1)
    {
        printf("get shmget failed\n");
        exit(1);
    }
    shm1 = (struct buffers *)shmat(shmid, nullptr, SHM_R|SHM_W);
    memset(shm1->buf, '\0', BUFSIZE);
    //获取信号灯
    if ((semid = semget(KEY, 6, 0666)) == -1)
    {
        printf("get semget failed\n");
        exit(2);
    }
    printf("in get semid = %d\n", semid);
    if((srcFile = fopen(srcFilePath, "rb")) == nullptr)
    {
        printf("get failed to open source file\n");
        exit(3);
    }
    fseek(srcFile, 0, SEEK_END);
    long int fSize = ftell(srcFile);
    if (fSize % BUFSIZE == 0)
    {
        i = fSize/BUFSIZE;
    }
    else {
        i = fSize/BUFSIZE + 1;
    }
    rewind(srcFile);
    QTimer * timer2 = new QTimer(this);
    timer2->start(10);
    //connect(timer2, SIGNAL(timeout()), this, SLOT(display()));
    timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(mget()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mget()
{
    static QString str;
    int j = 0;
    char buf[BUFSIZE + 1];
    memset(buf, '\0', BUFSIZE + 1);
    int count;
    if (i-- < 1)
    {
        timer->stop();
        fclose(srcFile);
        exit(0);
    }
    P(semid, 0);
    if ((count = fread(buf, 1, 10, srcFile)) == 0)
        printf("get fread error\n");
    shm1->count = count;
    while (j < shm1->count) {
        shm1->buf[j] = buf[j];
        j++;
    }
    buf[j] = '\0';
    str.append(QString(buf));
    ui->textBrowser->setText(str);
    V(semid, 1);
}

//void MainWindow::display()
//{
//    char buf[BUFSIZE + 1];
//    int j = 0;
//    memset(buf, '\0', BUFSIZE + 1);
//    //P(semid, 4);
//    while (j < shm1->count) {
//        buf[j] = shm1->buf[j];
//        j++;
//    }
//    buf[j] = '\0';
//    //V(semid, 4);
//    ui->label->setText(buf);
//}
