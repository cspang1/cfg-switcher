#include <QAbstractEventDispatcher>
#include <Windows.h>
#include <QMessageBox>
#include <QFileDialog>
#include "cfgswitcher.h"
#include "ui_cfgswitcher.h"
#include "Settings.h"
#include "gamemodel.h"
#include "gamepicker.h"
#include "game.h"

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent), gameModel(parent), ui(new Ui::CfgSwitcher) {
    if(!settings.initSettings()) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to initialize settings"));
        QApplication::exit(EXIT_FAILURE);
    }

    connect(&settings, SIGNAL(gameAdded()), &gameModel, SLOT(updateGamesView()));

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
}

void CfgSwitcher::on_setBattCfgBtn_clicked()
{
    if(!settings.setConfigs(0)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to set battery configuration files"));

    }
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
