#include "mainwindow.h"
#include <QApplication>

char * srcFilePath;
char * desFilePath;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    srcFilePath = argv[1];
    desFilePath = argv[2];
    MainWindow w;
    w.move(1000, 0);
    w.show();

    return a.exec();
}
