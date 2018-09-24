#include "mainwindow.h"
#include <QAbstractEventDispatcher>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    CurrentACStatus = getPowerStatus();
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

BYTE MainWindow::getPowerStatus() {
    SYSTEM_POWER_STATUS lpSystemPowerStatus;
    if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
        //std::string errMsg = "Error getting system power status: " + GetLastErrorAsString();
        //std::cerr << errMsg << std::endl;
        //return EXIT_FAILURE;
    }

    return lpSystemPowerStatus.ACLineStatus;
}
