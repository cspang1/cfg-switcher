#include <QMessageBox>
#include "settings.h"

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
