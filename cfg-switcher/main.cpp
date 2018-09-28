#include <QApplication>
#include "cfgswitcher.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CfgSwitcher w;
    w.show();

    return a.exec();
}
