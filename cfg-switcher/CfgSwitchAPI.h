#pragma once
#include "tinyxml2.h"

enum powerState { BATTERY, MAIN };

void deleteSettingsFile();
bool initSettings();
bool createSettingsFile();
bool createFileStruct(std::string rootDir);
bool initGames();
bool switchCfgs(powerState);
