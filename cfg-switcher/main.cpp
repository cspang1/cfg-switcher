#include <QApplication>
#include <QMessageBox>
#include "cfgswitcher.h"
#include "Settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Settings settings;
    if(!settings.initSettings()) {
        QMessageBox errMsg;
        errMsg.setText("ERROR: UNABLE TO INITIALIZE SETTINGS");
        errMsg.exec();
        return EXIT_FAILURE;
    }

    if(!settings.addGame("GTA V", "C:\\Users\\unkno\\Desktop\\test.xml")) {
        QMessageBox errMsg;
        errMsg.setText("ERROR: UNABLE TO ADD GAME");
        errMsg.exec();
        return EXIT_FAILURE;
    }

    CfgSwitcher w;
    w.show();

    return a.exec();
}
