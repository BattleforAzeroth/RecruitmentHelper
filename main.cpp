#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("秋招神器");
    QApplication::setOrganizationName("BattleforAzeroth");

    // 创建并显示主窗口
    MainWindow w;
    w.show();

    return a.exec();
}