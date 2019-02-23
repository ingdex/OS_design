/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QLabel *label_2;
    QLabel *label_5;
    QLabel *label_3;
    QLabel *hostNameDisplay;
    QLabel *sysBootTimeDisplay;
    QLabel *label;
    QLabel *label_4;
    QLabel *sysVersonDisplay;
    QLabel *sysRunTimeDisplay;
    QLabel *cpuModelDisplay;
    QWidget *tab_2;
    QTableWidget *tableWidget;
    QWidget *tab_3;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(610, 519);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 0, 591, 441));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 60, 91, 17));
        label_5 = new QLabel(tab);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(20, 180, 111, 17));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 100, 91, 17));
        hostNameDisplay = new QLabel(tab);
        hostNameDisplay->setObjectName(QString::fromUtf8("hostNameDisplay"));
        hostNameDisplay->setGeometry(QRect(130, 20, 300, 17));
        sysBootTimeDisplay = new QLabel(tab);
        sysBootTimeDisplay->setObjectName(QString::fromUtf8("sysBootTimeDisplay"));
        sysBootTimeDisplay->setGeometry(QRect(130, 60, 300, 17));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 51, 17));
        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 140, 91, 17));
        sysVersonDisplay = new QLabel(tab);
        sysVersonDisplay->setObjectName(QString::fromUtf8("sysVersonDisplay"));
        sysVersonDisplay->setGeometry(QRect(130, 140, 300, 17));
        sysRunTimeDisplay = new QLabel(tab);
        sysRunTimeDisplay->setObjectName(QString::fromUtf8("sysRunTimeDisplay"));
        sysRunTimeDisplay->setGeometry(QRect(130, 100, 300, 17));
        cpuModelDisplay = new QLabel(tab);
        cpuModelDisplay->setObjectName(QString::fromUtf8("cpuModelDisplay"));
        cpuModelDisplay->setGeometry(QRect(130, 180, 361, 141));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cpuModelDisplay->sizePolicy().hasHeightForWidth());
        cpuModelDisplay->setSizePolicy(sizePolicy);
        cpuModelDisplay->setLayoutDirection(Qt::LeftToRight);
        cpuModelDisplay->setInputMethodHints(Qt::ImhNone);
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tableWidget = new QTableWidget(tab_2);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 40, 571, 351));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        tabWidget->addTab(tab_3, QString());
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 610, 28));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\347\263\273\347\273\237\345\220\257\345\212\250\346\227\266\351\227\264", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "cpu\345\236\213\345\217\267\345\222\214\344\270\273\351\242\221", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\346\214\201\347\273\255\350\277\220\350\241\214\346\227\266\351\227\264", nullptr));
        hostNameDisplay->setText(QApplication::translate("MainWindow", "hostName", nullptr));
        sysBootTimeDisplay->setText(QApplication::translate("MainWindow", "sysBootTime", nullptr));
        label->setText(QApplication::translate("MainWindow", "\344\270\273\346\234\272\345\220\215", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "\347\263\273\347\273\237\347\232\204\347\211\210\346\234\254\345\217\267", nullptr));
        sysVersonDisplay->setText(QApplication::translate("MainWindow", "sysVerson", nullptr));
        sysRunTimeDisplay->setText(QApplication::translate("MainWindow", "sysRunTime", nullptr));
        cpuModelDisplay->setText(QApplication::translate("MainWindow", "cpuModel", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "\347\263\273\347\273\237\344\277\241\346\201\257", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "\350\277\233\347\250\213", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "\346\223\215\344\275\234", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
