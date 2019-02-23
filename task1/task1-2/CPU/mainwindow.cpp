#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <fstream>
#include <iostream>
#include <string>
#include <QEventLoop>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CPU utilization");
    QTimer *timer = new QTimer(this);//timer
    timer->start(2000);//2s
    connect(timer, SIGNAL(timeout()), this, SLOT(CPUUtilizationDisplay()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CPUUtilizationDisplay()
{
    QString str;
    string cpu;
    long int user, nice, sys, idle, iowait, irq, softirq;
    long int idle1, idle2, total1, total2;
    long int totalInterval;
    long int idleInterval;
    int utilization;
    std::ifstream in("/proc/stat");
    if(!in.is_open())
    {
        cout << "failed to open systerm file" << endl;
    }
    //get cpu usage for the first time
    in >> cpu >> user >> nice >> sys >> idle >> iowait >> irq >> softirq;
    idle1 = idle;
    total1 = user + nice + sys + idle + iowait + irq + softirq;
    //wait for 10ms
    QEventLoop eventLoop;
    QTimer::singleShot(10, &eventLoop, SLOT(quit()));
    eventLoop.exec();
    //get cpu usage for the second time
    in.seekg(in.beg);
    in >> cpu >> user >> nice >> sys >> idle >> iowait >> irq >> softirq;
    idle2 = idle;
    total2 = user + nice + sys + idle + iowait + irq + softirq;
    //calculate CPU utilization
    totalInterval = total2 - total1;
    idleInterval = idle2 - idle1;
    utilization = (int)(100 * (totalInterval - idleInterval) / totalInterval);
    //display CPU utilizaton
    ui->CPUUtilizationDisplay->setText(str.number(utilization, 10));
}
