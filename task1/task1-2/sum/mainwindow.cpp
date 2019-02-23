#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    ui->textBrowser->setText("1");
    ui->sumDisplay->setNum(1);
    timer->start(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(sum()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sum()
{
    static QString str = QString::number(1, 10);
    static int i = 2;
    static int sum = 1;
    if (i > 99)
        timer->stop();
    sum += i;
    str.append(" + " + QString::number(i));
    i++;
    ui->textBrowser->setText(str);
    ui->sumDisplay->setNum(sum);
}
