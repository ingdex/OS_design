#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <QDateTime>
#include <dirent.h>
#include <sys/stat.h>

//#include <QLayout>

#define BUFSIZE 1024

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Task Manager");
    hostName();
    sysBootTime();
    sysRunTime();
    sysVersion();
    cpuModel();
    processInfo();
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
    char host_name[BUFSIZE];
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

void MainWindow::sysRunTime()
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
    char ostype[BUFSIZE], osrelease[BUFSIZE];
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

void MainWindow::processInfo()
{
    DIR *dp;
    struct dirent *entry;
    struct stat statBuf;
    QString statPath;
    ifstream statFile;
    char name[BUFSIZE];
    int pid, ppid, rss, priority;
    QStringList headerLabels;
    QTableWidgetItem *nameItem;
    QTableWidgetItem *pidItem;
    QTableWidgetItem *ppidItem;
    QTableWidgetItem *rssItem;
    QTableWidgetItem *priorityItem;
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(5);
    headerLabels << "进程名" << "pid" << "ppid" << "占用内存/KB" << "优先级";
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("Jan"));
    ui->tableWidget->setColumnWidth(0, 120);
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
            if(strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)    //目录项的名字是”..”或”.”
            {
                //ignore the dir
                continue;
            }
            statPath = QString("%1%2%3").arg("/proc/").arg(entry->d_name).arg("stat");

            if (statPath == -1)
            {
                cout << "failed to open " << statPath.toStdString() << endl;
            }
            //get process info from /proc/%d/stat
            statFile >> pid;
            statFile >> name;


            nameItem = new QTableWidgetItem(entry->d_name);
            ui->tableWidget->setItem(i, 0, nameItem);
            pidItem = new QTableWidgetItem(entry->d_name);
            ui->tableWidget->setItem(i, 0, nameItem);
        }
        else    //ignore the file
        {
            continue;
        }

    }
}
