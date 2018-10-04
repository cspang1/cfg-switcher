#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <Windows.h>
#include <QAbstractEventDispatcher>
#include <QTimer>
#include "cfgswitcher.h"
#include "ui_cfgswitcher.h"
#include "gamemodel.h"
#include "gamepicker.h"
#include "game.h"
#include "gameheader.h"
#include "gamedelegate.h"

CfgSwitcher::CfgSwitcher(QWidget *parent) :
    QWidget(parent), gameModel(parent), ui(new Ui::CfgSwitcher) {
    // Load games from settings
    qRegisterMetaTypeStreamOperators<Game>("Game");
    QList<Game> games = settings.getGames();

    // Initialize UI
    ui->setupUi(this);
    setGameBtns(false);
    setFixedSize(600, 400);
    setWindowFlags(this->windowFlags() |= Qt::MSWindowsFixedSizeDialogHint);

    // Configure game table view model
    for(Game &g : games)
        addGame(g);
    ui->gamesTableView->setModel(&gameModel);
    GameHeader* header = new GameHeader(Qt::Horizontal, ui->gamesTableView);
    header->setStretchLastSection(true);
    ui->gamesTableView->setHorizontalHeader(header);
    for(int i = 0; i < gameModel.columnCount(); i++)
        if(i != 2)
            ui->gamesTableView->resizeColumnToContents(i);
    ui->gamesTableView->setItemDelegate(new GameDelegate());
    ui->gamesTableView->setFocusPolicy(Qt::NoFocus);
    QPalette palette = ui->gamesTableView->palette();
    palette.setBrush(QPalette::Highlight,QBrush(Qt::white));
    palette.setBrush(QPalette::HighlightedText,QBrush(Qt::black));
    ui->gamesTableView->setPalette(palette);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->setSectionResizeMode(4, QHeaderView::Fixed);
    header->setSectionResizeMode(5, QHeaderView::Fixed);

    // Configure signal/slot connections
    connect(header, SIGNAL(checkBoxClicked(Qt::CheckState)), &gameModel, SLOT(selectAll(Qt::CheckState)));
    connect(&gameModel, SIGNAL(setSelectAll(bool)), header, SLOT(setSelectAll(bool)));
    connect(&gameModel, SIGNAL(setGameBtns(bool)), this, SLOT(setGameBtns(bool)));

    // Create tray icon
    QSystemTrayIcon* m_tray_icon = new QSystemTrayIcon(QIcon(":/icons/resources/cfg_switcher.ico"), this);
    m_tray_icon->setToolTip("Config Switcher");
    connect(m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onShowHide(QSystemTrayIcon::ActivationReason)));
    QAction *quit_action = new QAction("Exit", m_tray_icon);
    connect(quit_action, SIGNAL(triggered()), this, SLOT(on_quitButton_clicked()));
    QAction *hide_action = new QAction("Show/Hide", m_tray_icon);
    connect(hide_action, SIGNAL(triggered()), this, SLOT(onShowHide()));
    QMenu *tray_icon_menu = new QMenu;
    tray_icon_menu->addAction(hide_action);
    tray_icon_menu->addAction(quit_action);
    m_tray_icon->setContextMenu(tray_icon_menu);
    m_tray_icon->show();

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

void CfgSwitcher::changeEvent(QEvent* e)
{
    switch (e->type())
    {
        case QEvent::LanguageChange:
            this->ui->retranslateUi(this);
            break;
        case QEvent::WindowStateChange:
            {
                if (this->windowState() & Qt::WindowMinimized)
                        QTimer::singleShot(0, this, SLOT(hide()));
                break;
            }
        default:
            break;
    }

    QWidget::changeEvent(e);
}

void CfgSwitcher::onShowHide(QSystemTrayIcon::ActivationReason reason) {
   if(reason != QSystemTrayIcon::DoubleClick)
       return;

   if(isVisible())
       hide();
   else {
       show();
       raise();
       setFocus();
   }
}
