#include <QAbstractEventDispatcher>
#include <Windows.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "cfgswitcher.h"
#include "ui_cfgswitcher.h"
#include "settings.h"
#include "gamemodel.h"
#include "gamepicker.h"
#include "game.h"
#include "checkboxheader.h"

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent), gameModel(parent), ui(new Ui::CfgSwitcher) {
    if(!settings.initSettings()) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to initialize settings"));
        QApplication::exit(EXIT_FAILURE);
    }

    // Initialize UI
    ui->setupUi(this);
    ui->remGames->setEnabled(false);

    // Initialize power status
    CurrentACStatus = getPowerStatus();
    setPowerStatusLabel();
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);

    // Initialize settings and games
    for(game &g : settings.getGames())
        addGame(QString::fromStdString(g.ID), QString::fromStdString(g.cfgPath));

    // Configure game table view model
    ui->gamesTableView->setModel(&gameModel);
    CheckboxHeader* header = new CheckboxHeader(Qt::Horizontal, ui->gamesTableView);
    header->setStretchLastSection(true);
    ui->gamesTableView->setHorizontalHeader(header);
    ui->gamesTableView->resizeColumnToContents(0);
    ui->gamesTableView->resizeColumnToContents(1);

    // Configure signal/slot connections
    connect(header, SIGNAL(checkBoxClicked(Qt::CheckState)), &gameModel, SLOT(selectAll(Qt::CheckState)));
    connect(&gameModel, SIGNAL(setSelectAll(bool)), header, SLOT(setSelectAll(bool)));
    connect(&gameModel, SIGNAL(setRemGameBtn(bool)), this, SLOT(setRemGameBtn(bool)));
}

void CfgSwitcher::addGame(QString gameName, QString gamePath) {
    QPair<QString, QString> pair(gameName, gamePath);
    gameModel.insertRows(0, 1, QModelIndex());
    QModelIndex index = gameModel.index(0, 0, QModelIndex());
    gameModel.setData(index, Qt::Unchecked, Qt::CheckStateRole);
    index = gameModel.index(0, 1, QModelIndex());
    gameModel.setData(index, gameName, Qt::EditRole);
    index = gameModel.index(0, 2, QModelIndex());
    gameModel.setData(index, gamePath, Qt::EditRole);
    ui->gamesTableView->resizeColumnToContents(1);
}

void CfgSwitcher::removeGame(QString gameName) {
    QList<QPair<QString, QString>> gameList = gameModel.getGames();
    int remIndex = -1;
    for(int i = 0; i < gameList.size(); i++)
        if(!gameName.compare(gameList.at(i).first))
            remIndex = i;

    gameModel.removeRows(remIndex, 1, QModelIndex());
    ui->gamesTableView->resizeColumnToContents(1);
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


void CfgSwitcher::on_remGames_clicked()
{
    QList<Qt::CheckState> selected = gameModel.getSelects();
    int index;
    while((index = selected.indexOf(Qt::Checked)) != -1) {
        QString gameName = gameModel.getGames().at(index).first;
        if(settings.removeGame(gameName.toStdString()))
            removeGame(gameName);
        selected = gameModel.getSelects();
    }
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

void CfgSwitcher::setRemGameBtn(bool state) {
    ui->remGames->setEnabled(state);
}
