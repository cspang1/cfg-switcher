#pragma once
#include <string>
#include <vector>
#include "game.h"
#include "CfgSwitchAPI.h"

class Settings {
	std::vector<game> games;
	std::string path;
	std::string cfgPath;
public:
	void deleteSettingsFile();
	bool initSettings();
	bool createSettingsFile();
	bool createFileStruct();
	bool updateFileStruct();
	bool cfgsSet();
	bool addGame(std::string gameID, std::string gamePath);
	bool gameExists(std::string gameID);
	bool setConfigs(powerState tgtState);
	std::vector<game> unsetGames();
	std::vector<game> getGames();
};
