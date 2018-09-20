#pragma once

enum powerState { MAIN, BATTERY };

bool initGames();
bool switchCfgs(powerState);
