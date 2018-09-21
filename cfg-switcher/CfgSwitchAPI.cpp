#include <iostream>
#include "CfgSwitchAPI.h"
#include "Settings.h"
#include "WinUtils.h"
#include "game.h"

bool switchConfigs(powerState pState, Settings &settings, game &game) {
	std::string cfgPath = settings.getCfgPath();
	std::string cfgSrc;
	std::string cfgFile;

	if (!game.battCfgSet || !game.mainCfgSet) {
		std::cerr << "Error: Can't switch " << game.ID << " config files; one or both config files not set" << std::endl;
		return false;
	}
	std::cout << "Switching " << game.ID << " config files to " << std::string(pState ? "plugged in" : "unplugged") + "..." << std::endl;
	cfgFile = FileFromPath(game.cfgPath);
	switch (pState) {
	case MAIN:
		cfgSrc = cfgPath + "\\" + game.ID + "\\main\\" + cfgFile;
		break;
	case BATTERY:
		cfgSrc = cfgPath + "\\" + game.ID + "\\battery\\" + cfgFile;
		break;
	default:
		std::cerr << "Error: Invalid AC line state specified" << std::endl;
		return false;
	}

	if (!copyFile(cfgSrc.c_str(), game.cfgPath.c_str())) {
		std::cerr << "Error: Unable to copy " << game.ID << " config file" << std::endl;
		return false;
	}

	return true;
}

bool switchConfigs(powerState pState, Settings &settings) {
	std::string cfgPath = settings.getCfgPath();
	std::string cfgSrc;
	std::string cfgFile;

	bool success = true;

	for (game &g : settings.getGames()) {
		if (!switchConfigs(pState, settings, g))
			success = false;
	}

	return success;
}