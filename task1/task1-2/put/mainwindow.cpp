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
    setWindowTitle("put");
    //获取共享内存组
    //buf2
    if ((shmid = shmget(KEY2, sizeof(buffers), IPC_CREAT|0666)) == -1)
    {
        printf("put shmget failed\n");
        exit(1);
    }
    shm2 = (struct buffers *)shmat(shmid, nullptr, SHM_R|SHM_W);
    memset(shm2->buf, '\0', BUFSIZE);
    //获取信号灯
    if ((semid = semget(KEY, 6, 0666)) == -1)
    {
        printf("put semget failed\n");
        exit(2);
    }
    printf("in put semid = %d\n", semid);
    if((srcFile = fopen(srcFilePath, "rb")) == nullptr)
    {
        printf("put failed to open source file\n");
        exit(3);
    }
    if((desFile = fopen(desFilePath, "wb")) == nullptr)
    {
        printf("put failed to open destination file\n");
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
    timer->start(2000);
    connect(timer, SIGNAL(timeout()), this, SLOT(mput()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mput()
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
        fclose(desFile);
        exit(0);
    }
    //P(semid, 5);
    P(semid, 3);
    while (j < shm2->count) {
        buf[j] = shm2->buf[j];
        j++;
    }
    buf[j] = '\0';
    count = shm2->count;
    memset(shm2->buf, '\0', BUFSIZE);
    shm2->count = 0;
    str.append(buf);
    ui->textBrowser->setText(str);
    fwrite(buf, 1, count, desFile);
    V(semid, 2);
}

//void MainWindow::display()
//{
//    char buf[BUFSIZE + 1];
//    int j = 0;
//    memset(buf, '\0', BUFSIZE + 1);
//    //P(semid, 5);
//    while (j < shm2->count) {
//        buf[j] = shm2->buf[j];
//        j++;
//    }
//    buf[j] = '\0';
//    //V(semid, 5);
//    ui->label->setText(buf);
//}
