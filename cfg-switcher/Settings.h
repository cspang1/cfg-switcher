#pragma once
#include <string>
#include <vector>
#include "game.h"

class Settings {
	std::vector<game> games;
	std::string path;
public:
	void deleteSettingsFile();
	bool initSettings();
	bool createSettingsFile();
	bool createFileStruct();
	bool addGame(std::string gameID);
	bool gameExists(std::string gameID);
	std::vector<game> getGames();
};
