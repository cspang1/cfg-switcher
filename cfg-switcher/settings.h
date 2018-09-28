#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDir>
#include "game.h"

class Settings : public QObject
{
    Q_OBJECT
private:
    const QSettings::Format CFG_FMT = QSettings::IniFormat;
    const QString GAME_GRP = "Games";
    const QString CFG_FILE = "config.ini";
    QSettings settings;
    bool updateFileStruct();
    inline QString gameCfgPath(Game game) { return CFG_PATH + "\\" + game.ID; }
    inline QString gameMainPath(Game game) { return gameCfgPath(game) + "\\main"; }
    inline QString gameBattPath(Game game) { return gameCfgPath(game) + "\\battery"; }
public:
    const QString CFG_PATH = QDir::currentPath() + "\\configs";
    explicit Settings(QObject *parent = nullptr) :
        QObject(parent), settings(CFG_FILE, CFG_FMT) {}
    QList<Game> getGames();
    bool addGame(Game game);
    void updateGame(Game game);
    bool removeGame(Game game);
    void enableGame(Game game);
    void disableGame(Game game);
    bool setGameConfig(int state, Game game);

signals:

public slots:
};

#endif // SETTINGS_H
