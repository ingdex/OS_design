#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QList>

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
    void sysVersion();
    void cpuModel();
    void displaySysRunTime();
    void displayProcessInfo();
    QLabel *curTimeLabel;
    QLabel *cpuUtilizationLabel;
    QLabel *memUtilizationLabel;
    void displayCurTime();
    void displayCpuUtilization();
    void displayMemUtilization();
    QList <int> cpuLineList;
    QList <int> memLineList;
    QList <int> swapLineList;
    void drawCpuLine();
    void drawMemLine();
    void drawSwapLine();
private slots:
    void updateSysRunTime();
    void processInfo();
    void queryProcess();
    void killProcess();
    void updateCurTime();
    void updateCpuUtilization();
    void updateMemUtilization();
    void updateCpuLine();
    void updateMemLine();
    void updateSwapLine();
    void updateCpuLineList();
    void updateMemLineList();
    void updateSwapLineList();
};

#endif // MAINWINDOW_H
