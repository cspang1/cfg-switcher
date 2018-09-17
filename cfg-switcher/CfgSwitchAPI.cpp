#include <iostream>
#include <vector>
#include "CfgSwitchAPI.h"
#include "tinyxml2.h"
#include "game.h"

std::vector<game> games;

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
	tinyxml2::XMLElement* element = settings.NewElement("version");
	element->SetText(0.1f);
	rootNode->InsertEndChild(element);
	element = settings.NewElement("games");

	// TESTING

	tinyxml2::XMLElement* gamesElement = settings.NewElement("game");
	tinyxml2::XMLElement* gameElement = settings.NewElement("id");
	gameElement->SetText("GTA V");
	gamesElement->InsertEndChild(gameElement);
	gameElement = settings.NewElement("cfgpath");
	gameElement->SetText("C:\\Users\\unkno\\Documents\\Rockstar Games\\GTA V");
	gamesElement->InsertEndChild(gameElement);
	element->InsertEndChild(gamesElement);

	gamesElement = settings.NewElement("game");
	gameElement = settings.NewElement("id");
	gameElement->SetText("Borderlands");
	gamesElement->InsertEndChild(gameElement);
	gameElement = settings.NewElement("cfgpath");
	gameElement->SetText("C:\\Users\\unkno\\Documents\\My Games\\Borderlands\\WillowGame\\Config");
	gamesElement->InsertEndChild(gameElement);
	element->InsertEndChild(gamesElement);

	rootNode->InsertEndChild(element);

	// TESTING

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
