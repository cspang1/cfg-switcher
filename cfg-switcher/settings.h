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
    const QString SET_GRP = "Settings";
    const QString MIN_SET = "StartMinimized";
    const QString RUN_SET = "RunAtStartup";
    const QString CFG_FILE = "config.ini";
    QSettings settings;
    bool updateFileStruct();

public:
    const QString GAME_CFG_PATH = QDir::currentPath() + "\\configs";
    explicit Settings(QObject *parent = nullptr);
    QList<Game> getGames();
    void setMin(bool min);
    bool getMin();
    void setRunStart(bool min);
    bool getRunStart();
    bool addGame(Game game);
    void updateGame(Game game);
    bool removeGame(Game game);
    void enableGame(Game game);
    void disableGame(Game game);
    bool setGameConfig(PowerState pState, Game game);
    bool switchable(Game game);
    inline QString gameCfgPath(Game game) { return GAME_CFG_PATH + "\\" + game.ID; }
    inline QString gameCfgPath(Game game, PowerState pState) { return gameCfgPath(game) + (pState == BATTERY ? "\\battery" : "\\main"); }
    inline QString gameCfgPath(Game game, PowerState pState, QString cfgFile) { return gameCfgPath(game, pState) + "\\" + cfgFile; }

signals:
public slots:
};

#endif // SETTINGS_H
