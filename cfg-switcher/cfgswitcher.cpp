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
    ui->setBattCfgBtn->setEnabled(false);
    ui->setMainCfgBtn->setEnabled(false);

    // Initialize power status
    CurrentACStatus = getPowerStatus();
    setPowerStatusLabel();
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);

    // Initialize settings and games
    for(Game &g : settings.getGames())
        addGame(g.ID, g.cfgPath, g.mainCfgSet, g.battCfgSet);

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
}

void CfgSwitcher::addGame(QString gameName, QString gamePath, bool mainCfgSet, bool battCfgSet) {
    gameModel.insertRows(0, 1, QModelIndex());
    QModelIndex index = gameModel.index(0, 0, QModelIndex());
    gameModel.setData(index, Qt::Unchecked, Qt::CheckStateRole);
    index = gameModel.index(0, 1, QModelIndex());
    gameModel.setData(index, gameName, Qt::EditRole);
    index = gameModel.index(0, 2, QModelIndex());
    gameModel.setData(index, gamePath, Qt::EditRole);
    index = gameModel.index(0, 3, QModelIndex());
    gameModel.setData(index, mainCfgSet, Qt::EditRole);
    index = gameModel.index(0, 4, QModelIndex());
    gameModel.setData(index, battCfgSet, Qt::EditRole);
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
    BYTE ACLineStatus = getPowerStatus();
    bool ACStatusChanged = false;
    MSG *msg = static_cast< MSG * >( message );
    if(msg->message == WM_POWERBROADCAST) {
        ACLineStatus = getPowerStatus();
        ACStatusChanged = CurrentACStatus != ACLineStatus;
        CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
        if (ACStatusChanged) {
            setPowerStatusLabel();
            switchConfigs(CurrentACStatus);
        }
    }
    return false;
}

bool CfgSwitcher::switchConfigs(int pState) {
    QString cfgPath = settings.getCfgPath();
    QString cfgSrc;
    QString cfgFile;

    bool success = true;

    for (Game &g : settings.getGames()) {
        if (!switchConfigs(pState, g))
            success = false;
    }

    return success;
}

bool CfgSwitcher::switchConfigs(int pState, Game &game) {
    QString cfgPath = settings.getCfgPath();
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

BYTE CfgSwitcher::getPowerStatus() {
    SYSTEM_POWER_STATUS lpSystemPowerStatus;
    if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to get system power status"));
        QApplication::exit(EXIT_FAILURE);
    }

    return lpSystemPowerStatus.ACLineStatus;
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
    setConfigs(1);
}

void CfgSwitcher::on_setBattCfgBtn_clicked()
{
    setConfigs(0);
}

bool CfgSwitcher::setConfigs(int pState) {
    QString stateStr = pState == 0 ? tr("battery") : tr("main");
    QList<Qt::CheckState> selects = gameModel.getSelects();
    QList<Game> games = gameModel.getGames();
    for(int row = 0; row < selects.size(); row++) {
        if(selects.at(row) == Qt::Checked) {
            if(!settings.setConfig(pState, games.at(row))) {
                QMessageBox::critical(this, tr("Error"), tr("Unable to set %1 configuration files").arg(stateStr));
                return false;
            }
            int col = pState == 0 ? 4 : 3;
            QModelIndex index = gameModel.index(row, col, QModelIndex());
            gameModel.setData(index, true, Qt::EditRole);
        }
    }

    gameModel.selectAll(Qt::Unchecked);
    QMessageBox::information(this, tr("Success!"), tr("Successfully set config files for %1 state").arg(stateStr), QMessageBox::Ok);

    return true;
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
        QString gameName = gameModel.getGames().at(index).ID;
        if(settings.removeGame(gameName))
            removeGame(gameName);
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
    GamePicker gamePicker(settings.getGames());
    QString gameName;
    QString gamePath;
    if(gamePicker.exec()) {
        gameName = gamePicker.getGameName();
        gamePath = gamePicker.getGamePath();

        if(settings.addGame(gameName, gamePath))
            addGame(gameName, gamePath, false, false);
        else
            QMessageBox::critical(this, tr("Error"), tr("Unable to add %1").arg(gameName));
    }
}

void CfgSwitcher::setGameBtns(bool state) {
    ui->remGames->setEnabled(state);
    ui->setBattCfgBtn->setEnabled(state);
    ui->setMainCfgBtn->setEnabled(state);
}
