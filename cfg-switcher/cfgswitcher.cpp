#include "cfgswitcher.h"
#include "ui_cfgswitcher.h"
#include <QAbstractEventDispatcher>
#include <Windows.h>

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CfgSwitcher)
{
    ui->setupUi(this);
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
                    ui->PowerStatus->setText("UNPLUGGED!");
                    break;
                case 1:
                    ui->PowerStatus->setText("PLUGGED IN!");
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

CfgSwitcher::~CfgSwitcher()
{
    delete ui;
}
