#pragma once
#include <vector>
#include "game.h"
#include "Settings.h"
#include "CfgSwitchAPI.h"

unsigned int __stdcall configSwitchThread(void* data);
BYTE getPowerStatus();
bool switchConfigs(powerState pState, Settings& settings);
