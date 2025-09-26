#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application information
    app.setApplicationName("秋招神器");
    app.setOrganizationName("AutumnRecruitHelper");
    app.setOrganizationDomain("example.com");

    // Create and show the main window
    MainWindow window;
    window.show();

    return app.exec();
}