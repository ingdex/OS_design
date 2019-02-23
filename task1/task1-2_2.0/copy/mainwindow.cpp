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
    //buf2
    if ((shmid = shmget(KEY2, sizeof(buffers), IPC_CREAT|0666)) == -1)
    {
        printf("copy shmget failed\n");
        exit(2);
    }
    shm2 = (struct buffers *)shmat(shmid, nullptr, SHM_R|SHM_W);
    //获取信号灯
    if ((semid = semget(KEY, 4, 0666)) == -1)
    {
        printf("copy semget failed\n");
        exit(3);
    }
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
    timer->start(500);
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
        QEventLoop eventLoop;
        QTimer::singleShot(5000, &eventLoop, SLOT(quit()));
        eventLoop.exec();
        exit(0);
    }
    P(semid, 1);
    count = shm1->cnum[shm1->out];
    while (j < count) {
        buf[j] = shm1->buf[shm1->out][j];
        j++;
    }
    shm1->out = (shm1->out + 1) % 10;
    buf[j] = '\0';
    V(semid, 0);
    P(semid, 2);
    j = 0;
    shm2->cnum[shm2->in] = count;
    while (j < count) {
        shm2->buf[shm2->in][j] = buf[j];
        j++;
    }
    shm2->in = (shm2->in + 1) % 10;
    V(semid, 3);
    str.append(buf);
    ui->textBrowser->setText(str);
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
