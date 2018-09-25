#include "cfgswitcher.h"
#include "ui_cfgswitcher.h"
#include <QAbstractEventDispatcher>
#include <Windows.h>
#include <QMessageBox>
#include "Settings.h"

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CfgSwitcher)
{
    QMessageBox errMsg;
    if(!settings.initSettings()) {
        errMsg.setText("ERROR: UNABLE TO INITIALIZE SETTINGS");
        errMsg.exec();
    }

    if(!settings.addGame("GTA V", "C:\\Users\\unkno\\Desktop\\test.xml")) {
        errMsg.setText("ERROR: UNABLE TO ADD GAME");
        errMsg.exec();
    }

    ui->setupUi(this);
    CurrentACStatus = getPowerStatus();
    setPowerStatusLabel();

    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

bool CfgSwitcher::nativeEventFilter(const QByteArray &, void *message, long *)
{
    BYTE ACLineStatus = getPowerStatus();
    bool ACStatusChanged = false;
    MSG *msg = static_cast< MSG * >( message );
    if(msg->message == WM_POWERBROADCAST) {
        ACLineStatus = getPowerStatus();
        ACStatusChanged = CurrentACStatus != ACLineStatus;
        CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
        if (ACStatusChanged)
            setPowerStatusLabel();
    }
    return false;
}

void CfgSwitcher::setPowerStatusLabel() {
    switch (CurrentACStatus) {
        case 0:
            ui->PowerStatus->setText("UNPLUGGED");
            break;
        case 1:
            ui->PowerStatus->setText("PLUGGED IN");
            break;
        default:
            //std::cerr << "Error: Invalid AC line status - " << GetLastErrorAsString() << std::endl;
            break;
    }
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


void CfgSwitcher::on_setMainCfgBtn_clicked()
{
    if(!settings.setConfigs(1)) {
        QMessageBox errMsg;
        errMsg.setText("ERROR: UNABLE TO SET MAIN CONFIGS");
        errMsg.exec();
    }
}

void CfgSwitcher::on_setBattCfgBtn_clicked()
{
    if(!settings.setConfigs(0)) {
        QMessageBox errMsg;
        errMsg.setText("ERROR: UNABLE TO SET BATTERY CONFIGS");
        errMsg.exec();
    }
}

void CfgSwitcher::on_quitButton_clicked()
{
    QCoreApplication::quit();
}
