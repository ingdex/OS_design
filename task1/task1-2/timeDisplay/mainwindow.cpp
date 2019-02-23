#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TimeDisplay");
    QTimer *timer = new QTimer(this);//计时器
    timer->start(10);//10ms
    connect(timer, SIGNAL(timeout()), this, SLOT(TimeUpDate()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TimeUpDate()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString timeStr = time.toString("yyyy-MM-dd hh:mm:ss ddd");
    ui->timeDisplay->setText(timeStr);
}
