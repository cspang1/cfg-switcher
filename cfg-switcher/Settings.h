#pragma once
#include <string>
#include <vector>
#include "game.h"

class Settings {
	std::vector<game> games;
	std::string path;
	std::string cfgPath;
    std::string settingsPath;
	bool createSettingsFile();
	bool createFileStruct();
	bool updateFileStruct();
public:
	Settings();
	bool initSettings();
	bool addGame(std::string gameID, std::string gamePath);
    bool removeGame(game remGame, int keep);
	bool gameExists(std::string gameID);
    bool setConfigs(int tgtState);
	std::vector<game> unsetGames();
	std::vector<game>& getGames() { return games; }
	std::string getPath() { return path; }
	std::string getCfgPath() { return cfgPath; }
};
