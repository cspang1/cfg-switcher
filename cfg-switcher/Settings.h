#pragma once
#include <string>
#include <vector>

class game;
enum powerState;

class Settings {
	std::vector<game> games;
	std::string path;
	std::string cfgPath;
public:
	void deleteSettingsFile();
	bool initSettings();
	bool createSettingsFile();
	bool createFileStruct();
	bool removeFileStruct(game remGame);
	bool updateFileStruct();
	bool addGame(std::string gameID, std::string gamePath);
	bool removeGame(game remGame, powerState keep);
	bool gameExists(std::string gameID);
	bool setConfigs(powerState tgtState);
	std::vector<game> unsetGames();
	std::vector<game>& getGames() { return games; }
	std::string getPath() { return path; }
	std::string getCfgPath() { return cfgPath; }
};
