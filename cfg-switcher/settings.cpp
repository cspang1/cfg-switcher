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

    return updateFileStruct();
}

void Settings::enableGame(Game game) {
    game.enabled = true;
    updateGame(game);
}

void Settings::disableGame(Game game) {
    game.enabled = false;
    updateGame(game);
}

bool Settings::setGameConfig(int tgtState, Game game) {
    QFileInfo fileInfo(QFile(game.cfgPath).fileName());
    QString cfgDest;
    switch (tgtState) {
        case 0:
            cfgDest = gameBattPath(game) + "\\" + fileInfo.fileName();
            break;
        case 1:
            cfgDest = gameMainPath(game) + "\\" + fileInfo.fileName();
            break;
    }

    if(QFile::exists(cfgDest))
        QFile::remove(cfgDest);
    if(!QFile::copy(game.cfgPath, cfgDest)) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Unable to set %1 config files").arg(game.ID));
        return false;
    }
    else {
        switch (tgtState) {
            case 0:
                game.battCfgSet = true;
                break;
            case 1:
                game.mainCfgSet = true;
        }
    }

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
        QString tempPath = gameMainPath(game);
        gmdCreate.mkpath(tempPath);
        gmdCreate.setPath(tempPath);
        tempPath = gameBattPath(game);
        gbdCreate.mkpath(tempPath);
        gbdCreate.setPath(tempPath);
        if (!gpdCreate.exists() || !gmdCreate.exists() || !gbdCreate.exists())
            QMessageBox::critical(nullptr, tr("Error"), tr("Unable to creat %1 config directories").arg(game.ID));
    }

    return true;
}
