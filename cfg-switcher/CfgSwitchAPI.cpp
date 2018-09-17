#include <iostream>
#include "CfgSwitchAPI.h"
#include "tinyxml2.h"

void deleteSettingsFile() {
	remove("settings.xml");
}

bool initSettings() {
	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
		if (!createSettingsFile()) {
			std::cerr << "Error creating new settings file..." << std::endl;
			return false;
		}
		else {
			loaded = settings.LoadFile("settings.xml");
			if (loaded != tinyxml2::XML_SUCCESS) {
				std::cerr << "Error loading settings file..." << std::endl;
				return false;
			}
		}
	}

	return true;
}

bool createSettingsFile() {
	tinyxml2::XMLDocument settings;
	tinyxml2::XMLNode* rootNode = settings.NewElement("settings");
	settings.InsertFirstChild(rootNode);
	tinyxml2::XMLElement* versionNode = settings.NewElement("version");
	versionNode->SetText(0.1f);
	rootNode->InsertEndChild(versionNode);
	tinyxml2::XMLElement* gamesNode = settings.NewElement("games");
	rootNode->InsertEndChild(gamesNode);
	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS)
		return false;
	return true;
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
