#include "cfgswitcher.h"
#include <Windows.h>
#include <QAbstractEventDispatcher>

CfgSwitcher::CfgSwitcher(QObject *parent) : QObject(parent)
{
    CurrentACStatus = getPowerStatus();
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

bool CfgSwitcher::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    BYTE ACLineStatus = getPowerStatus();
    bool ACStatusChanged = false;
    MSG *msg = static_cast< MSG * >( message );
    if(msg->message == WM_POWERBROADCAST) {
        ACLineStatus = getPowerStatus();
        ACStatusChanged = CurrentACStatus != ACLineStatus;
        CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
        if (ACStatusChanged) {
            switch (CurrentACStatus) {
                case 0:
                    //Msgbox.setText("UNPLUGGED");
                    break;
                case 1:
                    //Msgbox.setText("PLUGGED IN");
                    break;
                default:
                    //std::cerr << "Error: Invalid AC line status - " << GetLastErrorAsString() << std::endl;
                    break;
            }
            //Msgbox.exec();
        }
    }
    return false;
}

BYTE CfgSwitcher::getPowerStatus() {
    SYSTEM_POWER_STATUS lpSystemPowerStatus;
    if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
        //std::string errMsg = "Error getting system power status: " + GetLastErrorAsString();
        //std::cerr << errMsg << std::endl;
        //return EXIT_FAILURE;
    }

    return lpSystemPowerStatus.ACLineStatus;
}
