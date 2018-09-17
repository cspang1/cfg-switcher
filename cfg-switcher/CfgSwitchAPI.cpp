#include <iostream>
#include "CfgSwitchAPI.h"
#include "tinyxml2.h"

bool initSettings() {
	tinyxml2::XMLDocument settings;
	bool loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
		createSettings();
	}

	return true;
}

bool createSettings() {
	return false;
}

bool createFileStruct(std::string rootDir) {
	return false;
}

bool initGames() {
	return false;
}

bool switchCfgs(powerState) {
	return false;
}
