#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void hostName();
    void sysBootTime();
    void sysRunTime();
    void sysVersion();
    void cpuModel();
    void processInfo();
    void killProcess();
};

#endif // MAINWINDOW_H
