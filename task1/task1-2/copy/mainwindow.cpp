#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pv.h"
#include <stdio.h>
#include <QTimer>
#include <QEventLoop>

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
    setWindowTitle("copy");
    //获取共享内存组
    //buf1
    if ((shmid = shmget(KEY1, sizeof(buffers), IPC_CREAT|0666)) == -1)
    {
        printf("copy shmget failed\n");
        exit(1);
    }
    shm1 = (struct buffers *)shmat(shmid, nullptr, SHM_R|SHM_W);
    memset(shm1->buf, '\0', BUFSIZE);
    //buf2
    if ((shmid = shmget(KEY2, sizeof(buffers), IPC_CREAT|0666)) == -1)
    {
        printf("copy shmget failed\n");
        exit(2);
    }
    shm2 = (struct buffers *)shmat(shmid, nullptr, SHM_R|SHM_W);
    memset(shm2->buf, '\0', BUFSIZE);
    //获取信号灯
    if ((semid = semget(KEY, 6, 0666)) == -1)
    {
        printf("copy semget failed\n");
        exit(3);
    }
    printf("in copy semid = %d\n", semid);
    if((srcFile = fopen(srcFilePath, "rb")) == nullptr)
    {
        printf("copy failed to open source file\n");
        exit(4);
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
    timer = new QTimer(this);
    timer->start(1500);
    connect(timer, SIGNAL(timeout()), this, SLOT(mcopy()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mcopy()
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
    P(semid, 1);
    while (j < shm1->count) {
        buf[j] = shm1->buf[j];
        j++;
    }
    buf[j] = '\0';
    count = shm1->count;
    memset(shm1->buf, '\0', BUFSIZE);
    shm1->count = 0;
    V(semid, 0);
    P(semid, 2);
    j = 0;
    while (j < count) {
        shm2->buf[j] = buf[j];
        j++;
    }
    shm2->count = count;
    str.append(buf);
    ui->textBrowser->setText(str);
    V(semid, 3);
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
//    j = 0;
//    //P(semid, 5);
//    while (j < shm2->count) {
//        buf[j] = shm2->buf[j];
//        j++;
//    }
//    buf[j] = '\0';
//    //V(semid, 5);
//    ui->label_2->setText(buf);
//}
