#include "CfgSwitchAPI.h"

bool initSettings() {
	if (FILE *file = fopen("settings.xml", "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
	return false;
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
