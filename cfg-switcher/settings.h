#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include "game.h"

class Settings {
private:
    QString path;
    QString cfgPath;
    QString settingsPath;
    bool status;
	bool createSettingsFile();
    bool createFileStruct(QList<Game> games);
    bool updateFileStruct(QString gameID);

public:
    explicit Settings();
    QList<Game> initSettings();
    bool addGame(QString gameID, QString gamePath);
    bool removeGame(QString gameID);
    bool setConfig(int tgtState, Game game);
    bool setStatus(bool enabled, Game game);
    QString getPath() { return path; }
    QString getCfgPath() { return cfgPath; }
    bool initSuccess() { return status; }

public slots:
};

#endif // SETTINGS_H
