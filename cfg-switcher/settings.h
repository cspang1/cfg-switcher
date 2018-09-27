#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include "game.h"

class Settings {
private:
    QList<Game> games;
    QString path;
    QString cfgPath;
    QString settingsPath;
	bool createSettingsFile();
	bool createFileStruct();
	bool updateFileStruct();

public:
    explicit Settings();
	bool initSettings();
    bool addGame(QString gameID, QString gamePath);
    bool removeGame(QString gameID);
    bool gameExists(QString gameID);
    bool setConfigs(int tgtState);
    QList<Game> unsetGames();
    QList<Game>& getGames() { return games; }
    QString getPath() { return path; }
    QString getCfgPath() { return cfgPath; }

public slots:
};

#endif // SETTINGS_H
