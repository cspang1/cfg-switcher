#pragma once
#include <string>
#include <vector>
#include "game.h"

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
	bool addGame(std::string gameID, std::string gamePath);
	bool gameExists(std::string gameID);
	std::vector<game> getGames();
};
