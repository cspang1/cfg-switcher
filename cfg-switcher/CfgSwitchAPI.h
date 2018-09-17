#pragma once
#include <string>

enum powerState { BATTERY, MAIN };

bool initSettings();
bool createSettings();
bool createFileStruct(std::string rootDir);
bool initGames();
bool switchCfgs(powerState);
