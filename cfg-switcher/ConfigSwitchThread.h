#pragma once
#include "CfgSwitchAPI.h"

unsigned int __stdcall configSwitchThread(void* data);
BYTE getPowerStatus();
bool switchConfigs(powerState pState);
