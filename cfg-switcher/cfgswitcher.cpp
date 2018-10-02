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
#include "gameheader.h"

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent), gameModel(parent), ui(new Ui::CfgSwitcher) {
    // Load games from settings
    qRegisterMetaTypeStreamOperators<Game>("Game");
    QList<Game> games = settings.getGames();

    // Initialize UI
    ui->setupUi(this);
    setGameBtns(false);

    // Configure game table view model
    for(Game &g : games)
        addGame(g);
    ui->gamesTableView->setModel(&gameModel);
    GameHeader* header = new GameHeader(Qt::Horizontal, ui->gamesTableView);
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

CfgSwitcher::~CfgSwitcher()
{
    delete ui;
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

void CfgSwitcher::on_remGames_clicked()
{
    QList<Game> success, fail;
    QList<Qt::CheckState> selected = gameModel.getSelects();
    int index;
    while((index = selected.indexOf(Qt::Checked)) != -1) {
        Game game = gameModel.getGames().at(index);
        if(settings.removeGame(game)) {
            removeGame(game.ID);
            success.push_back(game);
        }
        else
            fail.push_back(game);
        selected = gameModel.getSelects();
    }
    setGameBtns(false);

    if(!success.isEmpty()) {
        QString resStr;
        for(Game g : success)
            resStr += g.ID + "\t";
        QMessageBox::information(this, tr("Success"), tr("Successfully removed:\n%1").arg(resStr), QMessageBox::Ok);
    }
    if(!fail.isEmpty()) {
        QString resStr;
        for(Game g : fail)
            resStr += g.ID + "\t";
        QMessageBox::critical(this, tr("Error"), tr("Failed to remove:\n%1").arg(resStr), QMessageBox::Ok);
    }
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

void CfgSwitcher::on_setMainCfgBtn_clicked()
{
    setConfigs(MAIN);
}

void CfgSwitcher::on_setBattCfgBtn_clicked()
{
    setConfigs(BATTERY);
}

void CfgSwitcher::setConfigs(PowerState pState) {
    QList<Game> success, fail;
    QString stateStr = pState == 0 ? tr("battery") : tr("main");
    QList<Qt::CheckState> selects = gameModel.getSelects();
    QList<Game> games = gameModel.getGames();
    for(int row = 0; row < selects.size(); row++) {
        if(selects.at(row) == Qt::Checked) {
            if(!settings.setGameConfig(pState, games.at(row))) {
                fail.push_back(games.at(row));
            }
            else {
                int col = pState == 0 ? 4 : 3;
                QModelIndex index = gameModel.index(row, col, QModelIndex());
                gameModel.setData(index, true, Qt::EditRole);
                success.push_back(games.at(row));
            }
        }
    }

    if(!success.isEmpty()) {
        QString resStr;
        for(Game g : success)
            resStr += g.ID + "\t";
        QMessageBox::information(this, tr("Success"), tr("Successfully set %1 config files for:\n%2").arg(stateStr).arg(resStr), QMessageBox::Ok);
    }
    if(!fail.isEmpty()) {
        QString resStr;
        for(Game g : fail)
            resStr += g.ID + "\t";
        QMessageBox::critical(this, tr("Error"), tr("Failed to set %1 config files for:\n%2").arg(stateStr).arg(resStr), QMessageBox::Ok);
    }

    gameModel.selectAll(Qt::Unchecked);
}

void CfgSwitcher::on_enableBtn_clicked() {
    setStatus(true);
}

void CfgSwitcher::on_disableBtn_clicked() {
    setStatus(false);
}

void CfgSwitcher::setStatus(bool status) {
    QList<Game> success, fail;
    QString stateStr = status ? tr("enabled") : tr("disabled");
    QList<Qt::CheckState> selects = gameModel.getSelects();
    QList<Game> games = gameModel.getGames();
    for(int row = 0; row < selects.size(); row++) {
        if(selects.at(row) == Qt::Checked) {
            Game game = games.at(row);
            if(game.enabled == status)
                continue;
            else if(status && (!game.battCfgSet || !game.mainCfgSet))
                fail.push_back(game);
            else {
                status ? settings.enableGame(game) : settings.disableGame(game);
                QModelIndex index = gameModel.index(row, 5, QModelIndex());
                gameModel.setData(index, status, Qt::EditRole);
                if(status)
                    switchConfigs(CurrentACStatus, game);
                success.push_back(game);
            }
        }
    }
    gameModel.selectAll(Qt::Unchecked);

    if(!success.isEmpty()) {
        QString resStr;
        for(Game g : success)
            resStr += g.ID + "\t";
        QMessageBox::information(this, tr("Success"), tr("Successfully %1:\n%2").arg(stateStr).arg(resStr), QMessageBox::Ok);
    }
    if(!fail.isEmpty()) {
        QString resStr;
        for(Game g : fail)
            resStr += g.ID + "\t";
        QMessageBox::critical(this, tr("Error"), tr("Unable to enable switching; config(s) unset for:\n%2").arg(resStr), QMessageBox::Ok);
    }
}

void CfgSwitcher::on_quitButton_clicked()
{
    QCoreApplication::quit();
}

void CfgSwitcher::setGameBtns(bool state) {
    ui->remGames->setEnabled(state);
    ui->setBattCfgBtn->setEnabled(state);
    ui->setMainCfgBtn->setEnabled(state);
    ui->enableBtn->setEnabled(state);
    ui->disableBtn->setEnabled(state);
}

bool CfgSwitcher::nativeEventFilter(const QByteArray &, void *message, long *)
{
    MSG *msg = static_cast<MSG*>(message);
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
    for (Game &game : gameModel.getGames())
        if(game.enabled)
            if (!switchConfigs(pState, game))
                success = false;

    return success;
}

bool CfgSwitcher::switchConfigs(PowerState pState, Game &game) {
    if (!settings.switchable(game)) {
        QMessageBox::warning(this, tr("Warning"), tr("One or both %1 config files not set; files won't be switched").arg(game.ID));
        return false;
    }
    QFileInfo cfgFileInfo(QFile(game.cfgPath));
    QString cfgSrc = settings.gameCfgPath(game, pState, cfgFileInfo.fileName());

    if(QFile::exists(game.cfgPath))
        QFile::remove(game.cfgPath);
    if(!QFile::copy(cfgSrc, game.cfgPath)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to copy %1 config files").arg(game.ID));
        return false;
    }

    return true;
}

void CfgSwitcher::setPowerStatusLabel() {
    ui->PowerStatus->setText((CurrentACStatus == BATTERY ? "UNPLUGGED" : "PLUGGED IN"));
}
