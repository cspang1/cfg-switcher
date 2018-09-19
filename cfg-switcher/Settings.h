#pragma once
#include <string>
#include <vector>
#include "game.h"

void deleteSettingsFile();
bool initSettings();
bool createSettingsFile();
bool createFileStruct();
bool addGame(std::string gameID);
std::vector<game> getGames();
