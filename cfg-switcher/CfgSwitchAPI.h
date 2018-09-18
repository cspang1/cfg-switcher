#pragma once
#include <string>
#include "tinyxml2.h"

enum powerState { BATTERY, MAIN };

void deleteSettingsFile();
bool initSettings();
bool createSettingsFile();
bool createFileStruct();
bool addGame(std::string gameID);
bool initGames();
bool switchCfgs(powerState);
