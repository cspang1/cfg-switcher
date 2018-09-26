#include <QAbstractEventDispatcher>
#include <Windows.h>
#include <QMessageBox>
#include <QFileDialog>
#include "cfgswitcher.h"
#include "ui_cfgswitcher.h"
#include "settings.h"
#include "gamemodel.h"
#include "gamepicker.h"
#include "game.h"

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent), gameModel(parent), ui(new Ui::CfgSwitcher) {
    if(!settings.initSettings()) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to initialize settings"));
        QApplication::exit(EXIT_FAILURE);
    }

    ui->setupUi(this);
    CurrentACStatus = getPowerStatus();
    setPowerStatusLabel();
    ui->gamesTableView->setModel(&gameModel);
    ui->gamesTableView->horizontalHeader()->setStretchLastSection(true);
    for(game &g : settings.getGames())
        addGame(QString::fromStdString(g.ID), QString::fromStdString(g.cfgPath));

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
        if (ACStatusChanged) {
            setPowerStatusLabel();
            switchConfigs(CurrentACStatus, settings);
        }
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
    }
}

BYTE CfgSwitcher::getPowerStatus() {
    SYSTEM_POWER_STATUS lpSystemPowerStatus;
    if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to get system power status"));
        QApplication::exit(EXIT_FAILURE);
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
        QMessageBox::critical(this, tr("Error"), tr("Unable to set main configuration files"));
        QApplication::exit(EXIT_FAILURE);
    }
    else
        QMessageBox::information(this, tr("Success!"), tr("Successfully set config files for main state"), QMessageBox::Ok);
}

void CfgSwitcher::on_setBattCfgBtn_clicked()
{
    if(!settings.setConfigs(0)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to set battery configuration files"));

    }
    else
        QMessageBox::information(this, tr("Success!"), tr("Successfully set config files for on-battery state"), QMessageBox::Ok);

}

void CfgSwitcher::on_quitButton_clicked()
{
    QCoreApplication::quit();
}

void CfgSwitcher::on_addGameBtn_clicked()
{
    GamePicker gamePicker;
    QString gameName;
    QString gamePath;
    if(gamePicker.exec()) {
        gameName = gamePicker.getGameName();
        gamePath = gamePicker.getGamePath();

        if(settings.addGame(gameName.toStdString(), gamePath.toStdString()))
            addGame(gameName, gamePath);
        else
            QMessageBox::critical(this, tr("Error"), tr("Unable to add %1").arg(gameName));
    }
}

void CfgSwitcher::addGame(QString gameName, QString gamePath) {
    QPair<QString, QString> pair(gameName, gamePath);
    gameModel.insertRows(0, 1, QModelIndex());
    QModelIndex index = gameModel.index(0, 0, QModelIndex());
    gameModel.setData(index, gameName, Qt::EditRole);
    index = gameModel.index(0, 1, QModelIndex());
    gameModel.setData(index, gamePath, Qt::EditRole);
}

bool CfgSwitcher::switchConfigs(int pState, Settings &settings, game &game) {
    std::string cfgPath = settings.getCfgPath();
    std::string cfgSrc;
    std::string cfgFile;

    if (!game.battCfgSet || !game.mainCfgSet) {
        //std::cerr << "Error: Can't switch " << game.ID << " config files; one or both config files not set" << std::endl;
        return false;
    }
    //std::cout << "Switching " << game.ID << " config files to " << std::string(pState ? "plugged in" : "unplugged") + "..." << std::endl;
    QFileInfo cfgFileInfo(QFile(QString::fromStdString(game.cfgPath)));
    cfgFile = cfgFileInfo.fileName().toStdString();
    QString filename(cfgFileInfo.fileName());
    switch (pState) {
    case 0:
        cfgSrc = cfgPath + "\\" + game.ID + "\\battery\\" + cfgFile;
        break;
    case 1:
        cfgSrc = cfgPath + "\\" + game.ID + "\\main\\" + cfgFile;
        break;
    default:
        //std::cerr << "Error: Invalid AC line state specified" << std::endl;
        return false;
    }

    //std::cout << cfgSrc << " to " << game.cfgPath << std::endl;

    if(QFile::exists(QString::fromStdString(game.cfgPath))) {
        QFile::remove(QString::fromStdString(game.cfgPath));
        //std::cout << "Removing " << game.cfgPath << std::endl;
    }
    if(!QFile::copy(QString::fromStdString(cfgSrc), QString::fromStdString(game.cfgPath))) {
        //std::cout << "Didn't work!" << std::endl;
        return false;
    }

    return true;
}

bool CfgSwitcher::switchConfigs(int pState, Settings &settings) {
    std::string cfgPath = settings.getCfgPath();
    std::string cfgSrc;
    std::string cfgFile;

    bool success = true;

    for (game &g : settings.getGames()) {
        if (!switchConfigs(pState, settings, g))
            success = false;
    }

    return success;
}
