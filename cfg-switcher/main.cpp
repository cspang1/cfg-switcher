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
    }

    CfgSwitcher w;
    w.show();

    return a.exec();
}
