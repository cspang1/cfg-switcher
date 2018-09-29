#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDir>
#include "game.h"
#include "powerstate.h"

class Settings : public QObject
{
    Q_OBJECT
private:
    const QSettings::Format CFG_FMT = QSettings::IniFormat;
    const QString GAME_GRP = "Games";
    const QString CFG_FILE = "config.ini";
    QSettings settings;
    bool updateFileStruct();

public:
    const QString CFG_PATH = QDir::currentPath() + "\\configs";
    explicit Settings(QObject *parent = nullptr) :
        QObject(parent), settings(CFG_FILE, CFG_FMT) {
        // Verify games in settings have file structs (via updateFileStructs?)
        // Match xxxCfgSet to files
        // Match enabled to xxxCfgSets
    }
    QList<Game> getGames();
    bool addGame(Game game);
    void updateGame(Game game);
    bool removeGame(Game game);
    void enableGame(Game game);
    void disableGame(Game game);
    bool setGameConfig(PowerState pState, Game game);
    inline QString gameCfgPath(Game game) { return CFG_PATH + "\\" + game.ID; }
    inline QString gameCfgPath(Game game, PowerState pState) { return gameCfgPath(game) + (pState == BATTERY ? "\\battery" : "\\main"); }
    inline QString gameCfgPath(Game game, PowerState pState, QString cfgFile) { return gameCfgPath(game, pState) + "\\" + cfgFile; }

signals:

public slots:
};

#endif // SETTINGS_H
