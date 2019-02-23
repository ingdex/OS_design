#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setText("Start");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    pid_t pid1, pid2, pid3;
    if ((pid1 = fork()) == 0)
    {//CPU utilization
        if (execl("./CPU", "./CPU", NULL) < 0)
        {
            exit(0);
        }
    }
    if ((pid2 = fork()) == 0)
    {//time display
        if (execl("./timeDisplay", "./timeDisplay", NULL) < 0)
        {
            exit(1);
        }
    }
    if ((pid3 = fork()) == 0)
    {//sum
        if (execl("./sum", "./sum", NULL) < 0)
        {
            exit(2);
        }
    }
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
}
