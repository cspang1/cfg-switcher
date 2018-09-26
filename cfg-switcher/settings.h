#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <string>
#include <vector>
#include "game.h"

class Settings : public QObject{
    Q_OBJECT
private:
	std::vector<game> games;
	std::string path;
	std::string cfgPath;
    std::string settingsPath;
	bool createSettingsFile();
	bool createFileStruct();
	bool updateFileStruct();

public:
    explicit Settings(QObject *parent = nullptr);
	bool initSettings();
	bool addGame(std::string gameID, std::string gamePath);
    bool removeGame(game remGame, int keep);
	bool gameExists(std::string gameID);
    bool setConfigs(int tgtState);
	std::vector<game> unsetGames();
	std::vector<game>& getGames() { return games; }
	std::string getPath() { return path; }
	std::string getCfgPath() { return cfgPath; }

public slots:
};

#endif // SETTINGS_H
