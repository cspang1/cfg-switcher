#include "cfgswitcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CfgSwitcher w;
    w.show();

    return a.exec();
}
