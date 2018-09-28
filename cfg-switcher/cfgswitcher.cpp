#include <QAbstractEventDispatcher>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QSettings>
#include <QDebug>
#include <Windows.h>
#include "cfgswitcher.h"
#include "ui_cfgswitcher.h"
#include "gamemodel.h"
#include "gamepicker.h"
#include "game.h"
#include "checkboxheader.h"

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent), gameModel(parent), ui(new Ui::CfgSwitcher) {

    // Get games manifest
    qRegisterMetaTypeStreamOperators<Game>("Game");
    QList<Game> games = settings.getGames();

    // Initialize UI
    ui->setupUi(this);
    setGameBtns(false);

    // Initialize settings and games
    for(Game &g : games)
        addGame(g);

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
    connect(&gameModel, SIGNAL(setGameBtns(bool)), this, SLOT(setGameBtns(bool)));

    // Initialize power status
    CurrentACStatus = getPowerState();
    setPowerStatusLabel();
    switchConfigs();
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

void CfgSwitcher::addGame(Game game) {
    gameModel.insertRows(0, 1, QModelIndex());
    QModelIndex index = gameModel.index(0, 0, QModelIndex());
    gameModel.setData(index, Qt::Unchecked, Qt::CheckStateRole);
    index = gameModel.index(0, 1, QModelIndex());
    gameModel.setData(index, game.ID, Qt::EditRole);
    index = gameModel.index(0, 2, QModelIndex());
    gameModel.setData(index, game.cfgPath, Qt::EditRole);
    index = gameModel.index(0, 3, QModelIndex());
    gameModel.setData(index, game.mainCfgSet, Qt::EditRole);
    index = gameModel.index(0, 4, QModelIndex());
    gameModel.setData(index, game.battCfgSet, Qt::EditRole);
    index = gameModel.index(0, 5, QModelIndex());
    gameModel.setData(index, game.enabled, Qt::EditRole);
    ui->gamesTableView->resizeColumnToContents(1);
}

void CfgSwitcher::removeGame(QString gameName) {
    QList<Game> games = gameModel.getGames();
    int remIndex = -1;
    for(int i = 0; i < games.size(); i++)
        if(!gameName.compare(games.at(i).ID))
            remIndex = i;

    gameModel.removeRows(remIndex, 1, QModelIndex());
    ui->gamesTableView->resizeColumnToContents(1);
}


bool CfgSwitcher::nativeEventFilter(const QByteArray &, void *message, long *)
{
    MSG *msg = static_cast< MSG * >( message );
    if(msg->message == WM_POWERBROADCAST) {
        PowerState ACLineStatus = getPowerState();
        bool ACStatusChanged = CurrentACStatus != ACLineStatus;
        CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
        if (ACStatusChanged) {
            setPowerStatusLabel();
            switchConfigs();
        }
    }
    return false;
}

PowerState CfgSwitcher::getPowerState() {
    SYSTEM_POWER_STATUS lpSystemPowerStatus;
    if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to get system power status"));
        QApplication::exit(EXIT_FAILURE);
    }

    return lpSystemPowerStatus.ACLineStatus == 0 ? BATTERY : MAIN;
}


bool CfgSwitcher::switchConfigs() {
    return switchConfigs(CurrentACStatus);
}

bool CfgSwitcher::switchConfigs(PowerState pState) {
    bool success = true;
    for (Game &g : gameModel.getGames())
        if(g.enabled)
            if (!switchConfigs(pState, g))
                success = false;

    return success;
}

bool CfgSwitcher::switchConfigs(PowerState pState, Game &game) {
    QString cfgPath = settings.CFG_PATH;
    QString cfgSrc;
    QString cfgFile;

    if (!game.battCfgSet || !game.mainCfgSet) {
        QMessageBox::warning(this, tr("Warning"), tr("One or both %1 config files not set; files won't be switched").arg(game.ID));
        return false;
    }
    QFileInfo cfgFileInfo(QFile(game.cfgPath));
    cfgFile = cfgFileInfo.fileName();
    QString filename(cfgFileInfo.fileName());
    switch (pState) {
    case 0:
        cfgSrc = cfgPath + "\\" + game.ID + "\\battery\\" + cfgFile;
        break;
    case 1:
        cfgSrc = cfgPath + "\\" + game.ID + "\\main\\" + cfgFile;
        break;
    }

    if(QFile::exists(game.cfgPath)) {
        QFile::remove(game.cfgPath);
    }
    if(!QFile::copy(cfgSrc, game.cfgPath)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to copy %1 config files").arg(game.ID));
        return false;
    }

    return true;
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

CfgSwitcher::~CfgSwitcher()
{
    delete ui;
}


void CfgSwitcher::on_setMainCfgBtn_clicked()
{
    setConfigs(MAIN);
}

void CfgSwitcher::on_setBattCfgBtn_clicked()
{
    setConfigs(BATTERY);
}

void CfgSwitcher::setConfigs(PowerState pState) {
    QString stateStr = pState == 0 ? tr("battery") : tr("main");
    QList<Qt::CheckState> selects = gameModel.getSelects();
    QList<Game> games = gameModel.getGames();
    bool success = true;
    for(int row = 0; row < selects.size(); row++) {
        if(selects.at(row) == Qt::Checked) {
            if(!settings.setGameConfig(pState, games.at(row))) { // NEED TO SET battCfg and mainCfg set vars in gameModel obj?
                QMessageBox::critical(this, tr("Error"), tr("Unable to set %1 configuration files").arg(stateStr));
                success = false;
            }
            int col = pState == 0 ? 4 : 3;
            QModelIndex index = gameModel.index(row, col, QModelIndex());
            gameModel.setData(index, true, Qt::EditRole);
        }
    }

    gameModel.selectAll(Qt::Unchecked);
    if(success)
        QMessageBox::information(this, tr("Success"), tr("Successfully set config files for %1 state").arg(stateStr), QMessageBox::Ok);
    else
        QMessageBox::information(this, tr("Error"), tr("Unable to set some or all config files"), QMessageBox::Ok);

}

void CfgSwitcher::on_quitButton_clicked()
{
    QCoreApplication::quit();
}


void CfgSwitcher::on_remGames_clicked()
{
    QList<Qt::CheckState> selected = gameModel.getSelects();
    int index;
    bool success = true;
    while((index = selected.indexOf(Qt::Checked)) != -1) {
        Game game = gameModel.getGames().at(index);
        if(settings.removeGame(game))
            removeGame(game.ID);
        else
            success = false;
        selected = gameModel.getSelects();
    }
    setGameBtns(false);
    if(!success)
        QMessageBox::critical(this, tr("Error"), tr("Failed to remove some or all games"), QMessageBox::Ok);
    else
        QMessageBox::information(this, tr("Success"), tr("Successfully removed game(s)"), QMessageBox::Ok);
}


void CfgSwitcher::on_addGameBtn_clicked()
{
    GamePicker gamePicker(gameModel.getGames());
    QString gameID;
    QString gamePath;
    if(gamePicker.exec()) {
        gameID = gamePicker.getGameName();
        gamePath = gamePicker.getGamePath();

        if(!gameModel.gameExists(gameID)) {
            if(settings.addGame(Game(gameID, gamePath)))
                addGame(Game(gameID, gamePath));
            else
                QMessageBox::information(this, tr("Error"), tr("%1 already exists in configuration").arg(gameID), QMessageBox::Ok);
        }
        else
            QMessageBox::critical(this, tr("Error"), tr("Unable to add %1").arg(gameID));
    }
}

void CfgSwitcher::setGameBtns(bool state) {
    ui->remGames->setEnabled(state);
    ui->setBattCfgBtn->setEnabled(state);
    ui->setMainCfgBtn->setEnabled(state);
    ui->enableBtn->setEnabled(state);
    ui->disableBtn->setEnabled(state);
}

void CfgSwitcher::on_enableBtn_clicked() {
    setStatus(true);
}

void CfgSwitcher::on_disableBtn_clicked() {
    setStatus(false);
}

void CfgSwitcher::setStatus(bool status) {
    QList<Qt::CheckState> selects = gameModel.getSelects();
    QList<Game> games = gameModel.getGames();
    bool success = true;
    for(int row = 0; row < selects.size(); row++) {
        if(selects.at(row) == Qt::Checked) {
            Game game = games.at(row);
            if(game.enabled == status)
                continue;
            else if(status && (!game.battCfgSet || !game.mainCfgSet)) {
                QMessageBox::information(this, tr("Error"), tr("%1 config(s) unset; unable to enable switching").arg(game.ID), QMessageBox::Ok);
                success = false;
            }
            else {
                status ? settings.enableGame(game) : settings.disableGame(game);
                QModelIndex index = gameModel.index(row, 5, QModelIndex());
                gameModel.setData(index, status, Qt::EditRole);
                if(status)
                    switchConfigs(CurrentACStatus, game);
            }
        }
    }

    gameModel.selectAll(Qt::Unchecked);
    QString stateStr = status ? tr("enabled") : tr("disabled");
    if(!success)
        QMessageBox::critical(this, tr("Error"), tr("Failed to enable some or all games"), QMessageBox::Ok);
    else
        QMessageBox::information(this, tr("Success"), tr("Successfully %1 game(s)").arg(stateStr), QMessageBox::Ok);

}


