#pragma once
class game;
class Settings;

enum powerState { BATTERY, MAIN, NONE };

bool switchConfigs(powerState pState, Settings &settings, game &game);
bool switchConfigs(powerState pState, Settings &settings);
