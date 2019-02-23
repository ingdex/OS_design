#include "mainwindow.h"
#include <QApplication>

char * srcFilePath;
char * desFilePath;
int count;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    srcFilePath = argv[1];
    desFilePath = argv[2];
    MainWindow w;
    w.move(0, 0);
    w.show();

    return a.exec();
}

