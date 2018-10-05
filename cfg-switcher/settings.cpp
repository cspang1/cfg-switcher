#include <QMessageBox>
#include <QCoreApplication>
#include <QStandardPaths>
#include "cfgswitcher.h"
#include "settings.h"

Settings::Settings(QObject *parent) :
       QObject(parent), settings(CFG_FILE, CFG_FMT) {
    // Check start minimized/start at run
    if(!QFileInfo::exists(CFG_FILE) || !QFileInfo(CFG_FILE).isFile()) {
        setMin(false);
        setRunStart(false);
    }

    // Verify games in settings have file structs (via updateFileStructs?)
    // Match xxxCfgSet to files
    // Match enabled to xxxCfgSets
}

QList<Game> Settings::getGames() {
    QList<Game> games;
    settings.beginGroup(GAME_GRP);
    for(QString game : settings.childKeys()) {
        QVariant g = settings.value(game);
        games.push_back(g.value<Game>());
    }
    settings.endGroup();
    return games;
}

void Settings::setMin(bool min) {
    settings.beginGroup(SET_GRP);
    settings.setValue(MIN_SET, min);
    settings.endGroup();
}

void Settings::setRunStart(bool start) {
    settings.beginGroup(SET_GRP);
    settings.setValue(RUN_SET, start);
    settings.endGroup();
    QFileInfo fileInfo(QCoreApplication::applicationFilePath());
    QString startupFilePath =
            QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + QDir::separator()
            + "Startup" + QDir::separator()
            + fileInfo.completeBaseName() + ".lnk";
    if(start)
        QFile::link(
                QCoreApplication::applicationFilePath(),
                startupFilePath
                );
    else
        QFile(startupFilePath).remove();
}

bool Settings::getMin() {
    settings.beginGroup(SET_GRP);
    QVariant m = settings.value(MIN_SET);
    settings.endGroup();
    return m.value<bool>();
}

bool Settings::getRunStart() {
    settings.beginGroup(SET_GRP);
    QVariant s = settings.value(RUN_SET);
    settings.endGroup();
    return s.value<bool>();
}


bool Settings::addGame(Game game) {
    updateGame(game);

    return updateFileStruct();
}

void Settings::updateGame(Game game) {
    settings.beginGroup(GAME_GRP);
    settings.setValue(game.ID, QVariant::fromValue(game));
    settings.endGroup();
}

bool Settings::removeGame(Game game) {
    settings.beginGroup(GAME_GRP);
    settings.remove(game.ID);
    settings.endGroup();

    if(!QDir(gameCfgPath(game)).removeRecursively()) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Unable to remove %1 config files").arg(game.ID));
        return false;
    }

    return true;
}

void Settings::enableGame(Game game) {
    game.enabled = true;
    updateGame(game);
}

void Settings::disableGame(Game game) {
    game.enabled = false;
    updateGame(game);
}

bool Settings::setGameConfig(PowerState pState, Game game) {
    QFileInfo fileInfo(QFile(game.cfgPath).fileName());
    QString cfgDest = gameCfgPath(game, pState, fileInfo.fileName());
    if(QFile::exists(cfgDest))
        QFile::remove(cfgDest);
    if(!QFile::copy(game.cfgPath, cfgDest)) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Unable to set %1 config files").arg(game.ID));
        return false;
    }
    else
        pState == BATTERY ? game.battCfgSet = true : game.mainCfgSet = true;

    updateGame(game);

    return true;
}

bool Settings::updateFileStruct() {
    for (Game game : getGames()) {
        QDir gpdCreate;
        QDir gmdCreate;
        QDir gbdCreate;
        QString gamePath = gameCfgPath(game);
        gpdCreate.mkpath(gamePath);
        gpdCreate.setPath(gamePath);
        QString tempPath = gameCfgPath(game, MAIN);
        gmdCreate.mkpath(tempPath);
        gmdCreate.setPath(tempPath);
        tempPath = gameCfgPath(game, BATTERY);
        gbdCreate.mkpath(tempPath);
        gbdCreate.setPath(tempPath);
        if (!gpdCreate.exists() || !gmdCreate.exists() || !gbdCreate.exists())
            QMessageBox::critical(nullptr, tr("Error"), tr("Unable to create %1 config directories").arg(game.ID));
    }

    return true;
}

bool Settings::switchable(Game game) {
    return game.mainCfgSet && game.battCfgSet;
}
