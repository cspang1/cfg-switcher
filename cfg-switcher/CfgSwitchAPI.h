#pragma once

enum powerState { BATTERY, MAIN };

bool initGames();
bool switchCfgs(powerState);
