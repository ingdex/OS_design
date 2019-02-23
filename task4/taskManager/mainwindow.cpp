#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <QDateTime>
#include <dirent.h>
#include <sys/stat.h>
#include <cctype>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QPainter>

#define BUF_SIZE 1024
#define MAP_WIDTH 600
#define MAP_HEIGHT 160

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Task Manager");
    hostName();
    sysBootTime();
    sysVersion();
    cpuModel();
    processInfo();
    displaySysRunTime();
    displayCurTime();
    displayCpuUtilization();
    displayMemUtilization();
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(processInfo()));
    connect(ui->queryButton, SIGNAL(clicked()), this, SLOT(queryProcess()));
    connect(ui->killButton, SIGNAL(clicked()), this, SLOT(killProcess()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hostName()
{
    ifstream ifile("/proc/sys/kernel/hostname");
    if (!ifile.is_open())
    {
        ui->hostNameDisplay->setText("failed to open");
        return;
    }
    char host_name[BUF_SIZE];
    ifile >> host_name;
    ui->hostNameDisplay->setText(host_name);
    ifile.close();
}

void MainWindow::sysBootTime()
{
    ifstream ifile("/proc/uptime");
    if (!ifile.is_open())
    {
        ui->sysBootTimeDisplay->setText("failed to open");
        return;
    }
    time_t sys_boot_time = 0;
    time_t sys_run_time = 0;
    time_t sys_cur_time = 0;
    ifile >> sys_run_time;
    time(&sys_cur_time);
    sys_boot_time = sys_cur_time - sys_run_time;
    QDateTime boot_time = QDateTime::fromTime_t(sys_boot_time);
    ui->sysBootTimeDisplay->setText(boot_time.toString("yyyy年MM月dd日  hh:mm:ss"));
    ifile.close();
}

void MainWindow::displaySysRunTime()
{
    QTimer *timer = new QTimer(this);//timer
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateSysRunTime()));
}

void MainWindow::updateSysRunTime()
{
    ifstream ifile("/proc/uptime");
    if (!ifile.is_open())
    {
        ui->sysRunTimeDisplay->setText("failed to open");
        return;
    }
    double sys_run_time;
    ifile >> sys_run_time;
    ui->sysRunTimeDisplay->setText(QString("%1%2").arg(sys_run_time).arg("s"));
    ifile.close();
}

void MainWindow::sysVersion()
{
    ifstream ifile1("/proc/sys/kernel/ostype");
    ifstream ifile2("/proc/sys/kernel/osrelease");
    if (!ifile1.is_open() || !ifile2.is_open())
    {
        ui->sysVersonDisplay->setText("failed to open");
        return;
    }
    char ostype[BUF_SIZE], osrelease[BUF_SIZE];
    ifile1 >> ostype;
    ifile2 >> osrelease;
    ui->sysVersonDisplay->setText(QString("%1%2").arg(ostype).arg(osrelease));
    ifile1.close();
    ifile2.close();
}

void MainWindow::cpuModel()
{
    ifstream ifile("/proc/cpuinfo");
    ui->cpuModelDisplay->setAlignment(Qt::AlignTop);
    if (!ifile.is_open())
    {
        ui->cpuModelDisplay->setText("failed to open");
        return;
    }
    string line, substr;
    QString model;
    while (getline(ifile, line)) {
        if ((substr = line.substr(0, 9)) == "processor")
        {
            substr = line.substr(12, 13);
            model.append(QString::fromStdString(substr));
            model.append(": ");
        }
        if ((substr = line.substr(0, 10)) == "model name")
        {
            substr = line.substr(12, line.length());
            model.append(QString::fromStdString(substr));
            model.append("\n");
        }
    }
    ui->cpuModelDisplay->setText(model);
    ifile.close();
}

void MainWindow::displayProcessInfo()
{

}

void MainWindow::processInfo()
{
    DIR *dp;
    struct dirent *entry;
    struct stat statBuf;
    QString statPath;
    ifstream statFile;
    string name;
    //char name[BUF_SIZE];
    int pid, ppid, rss, priority;
    int tempi;
    int j = 0;
    char tempc;
    QStringList headerLabels;
    QTableWidgetItem *nameItem;
    QTableWidgetItem *pidItem;
    QTableWidgetItem *ppidItem;
    QTableWidgetItem *rssItem;
    QTableWidgetItem *priorityItem;
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setColumnWidth(0, 120);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 100);
    ui->tableWidget->setColumnWidth(3, 100);
    ui->tableWidget->setColumnWidth(4, 100);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    headerLabels << "进程名" << "pid" << "ppid" << "占用内存/KB" << "优先级";
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    if  ((dp = opendir("/proc")) == nullptr)
    {
        //打印出错信息;
        cout << "目录/proc打开失败" << endl;
        //返回;
        exit(0);
    }
    int i = 0;
    while (entry = readdir(dp))//read files in directory /proc
    {
        lstat(entry->d_name, &statBuf);
        if (entry->d_type == DT_DIR)
        {
            if(!isdigit(*(entry->d_name)) || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)    //目录项的名字是”..”或”.”
            {
                //ignore the dir
                continue;
            }
            statPath = QString("%1/%2/%3").arg("/proc").arg(entry->d_name).arg("stat");
            statFile.open(statPath.toStdString());
            if (!statFile.is_open())
            {
                cout << "failed to open " << statPath.toStdString() << endl;
                continue;
            }
            ui->tableWidget->setRowCount(i + 1);
            //get process info from /proc/%d/stat
            statFile >> pid;
            statFile >> name;
            name = name.substr(1, name.length() - 2);
            statFile >> tempc;
            statFile >> ppid;
            for (j = 0; j < 13; j++)
            {
                statFile >>tempi;
            }
            statFile >> priority;
            for (j = 0; j < 5; j++)
            {
                statFile >>tempi;
            }
            statFile >> rss;
            nameItem = new QTableWidgetItem(QString::fromStdString(name));
            ui->tableWidget->setItem(i, 0, nameItem);
            nameItem->setTextAlignment(Qt::AlignCenter);
            pidItem = new QTableWidgetItem(QString::number(pid));
            ui->tableWidget->setItem(i, 1, pidItem);
            ppidItem = new QTableWidgetItem(QString::number(ppid));
            ui->tableWidget->setItem(i, 2, ppidItem);
            rssItem = new QTableWidgetItem(QString::number(rss));
            ui->tableWidget->setItem(i, 3, rssItem);
            priorityItem = new QTableWidgetItem(QString::number(priority));
            ui->tableWidget->setItem(i, 4, priorityItem);
            i++;
            statFile.close();
        }
        else    //ignore the file
        {
            continue;
        }
    }
    //display every row again after queryProcess()
    for (int j = 0; j < i; j++)
    {
        ui->tableWidget->setRowHidden(j, false);
    }
}

void MainWindow::queryProcess()
{
    QString key = ui->lineEdit->text();
    if (key == "")
    {
        return;
    }
    QList <QTableWidgetItem *> items = ui->tableWidget->findItems(key, Qt::MatchContains);
    int processCount = items.count();
    int rowCount = ui->tableWidget->rowCount();//get line count
    int i = 0;
    //hidden evrey row
    for (i = 0; i < rowCount; i++)
    {
        ui->tableWidget->setRowHidden(i, true);
    }
    //display matching processes
    for (i = 0; i < processCount; i++)
    {
        int row = items[i]->row();
        ui->tableWidget->setRowHidden(row, false);
    }
}

void MainWindow::killProcess()
{
    QTableWidget *tableWidget = ui->tableWidget;
    QTableWidgetItem *curItem = tableWidget->currentItem();
    if (curItem == nullptr)
    {
        ui->testlabel->setText("bug");
        return;
    }
    int row = curItem->row();
    QTableWidgetItem *pidItem = tableWidget->item(row, 1);
    QString pid = pidItem->text();//get pid
    //kill process
    QString command = QString("kill %1").arg(pid);
    system(command.toLatin1().data());
}

void MainWindow::displayCurTime()
{
    curTimeLabel = new QLabel;
    ui->statusBar->addPermanentWidget(curTimeLabel, 10);
    updateCurTime();
    QTimer *timer = new QTimer(this);//timer
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCurTime()));
}

void MainWindow::updateCurTime()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString timeStr = time.toString("    yyyy-MM-dd hh:mm:ss ddd");
    curTimeLabel->setText(timeStr);
}

void MainWindow::displayCpuUtilization()
{
    cpuUtilizationLabel = new QLabel;
    ui->statusBar->addPermanentWidget(cpuUtilizationLabel, 4);
    updateCpuUtilization();
    QTimer *timer = new QTimer(this);//timer
    timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCpuUtilization()));
}

void MainWindow::updateCpuUtilization()
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
    //wait for 100ms
    QEventLoop eventLoop;
    QTimer::singleShot(100, &eventLoop, SLOT(quit()));
    eventLoop.exec();
    //get cpu usage for the second time
    in.close();
    in.open("/proc/stat");
    in >> cpu >> user >> nice >> sys >> idle >> iowait >> irq >> softirq;
    idle2 = idle;
    total2 = user + nice + sys + idle + iowait + irq + softirq;
    //calculate CPU utilization
    totalInterval = total2 - total1;
    idleInterval = idle2 - idle1;
    utilization = (int)(100 * (totalInterval - idleInterval) / totalInterval);
    //display CPU utilizaton
    str = QString("CPU: %1%").arg(utilization);
    cpuUtilizationLabel->setText(str);
    in.close();
}

void MainWindow::displayMemUtilization()
{
    memUtilizationLabel = new QLabel;
    ui->statusBar->addPermanentWidget(memUtilizationLabel, 8);
    updateMemUtilization();
    QTimer *timer = new QTimer(this);//timer
    timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMemUtilization()));
}

void MainWindow::updateMemUtilization()
{
    string tmp;
    int memTotal, memFree;
    int utilization;
    QString str;
    std::ifstream in("/proc/meminfo");
    if(!in.is_open())
    {
        cout << "failed to open /proc/meminfo" << endl;
    }
    //get mem usage for the first time
    in >> tmp >> memTotal >> tmp >> tmp >> memFree;
    utilization = 100 * (memTotal - memFree) / memTotal;
    //display CPU utilizaton
    str = QString("Mem: %1%").arg(utilization);
    memUtilizationLabel->setText(str);
    in.close();
}

void MainWindow::drawCpuLine()
{
    QTimer *timer1 = new QTimer;
    timer1->start(200);
    connect(timer1, SIGNAL(timeout), this, SLOT(updateCpuLineList()));
    QTimer *timer2 = new QTimer;
    timer2->start(200);
    connect(timer2, SIGNAL(timeout), this, SLOT(updateCpuLine()));
}

void MainWindow::updateCpuLine()
{
    QPixmap pixmap(MAP_WIDTH, MAP_HEIGHT);
    QPainter painter(&pixmap);
    pixmap.fill(Qt::white);
    QPen pen;
    pen.setColor(Qt::gray);
    for (int i=1; i<5; i++)
    {
        painter.drawLine(0, i*MAP_HEIGHT/5, MAP_WIDTH, i*MAP_HEIGHT/5);
    }
    pen.setColor(Qt::red);
    pen.setWidth(2);
}

void MainWindow::updateCpuLineList()
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
    //wait for 100ms
    QEventLoop eventLoop;
    QTimer::singleShot(100, &eventLoop, SLOT(quit()));
    eventLoop.exec();
    //get cpu usage for the second time
    in.close();
    in.open("/proc/stat");
    in >> cpu >> user >> nice >> sys >> idle >> iowait >> irq >> softirq;
    idle2 = idle;
    total2 = user + nice + sys + idle + iowait + irq + softirq;
    //calculate CPU utilization
    totalInterval = total2 - total1;
    idleInterval = idle2 - idle1;
    utilization = (int)(totalInterval - idleInterval) / totalInterval;
    //display CPU utilizaton
    cpuLineList.push_back(utilization * MAP_HEIGHT);
    in.close();
}

void MainWindow::drawMemLine()
{

}

void MainWindow::updateMemLine()
{

}

void MainWindow::updateMemLineList()
{

}

void MainWindow::drawSwapLine()
{

}

void MainWindow::updateSwapLine()
{

}

void MainWindow::updateSwapLineList()
{

}
