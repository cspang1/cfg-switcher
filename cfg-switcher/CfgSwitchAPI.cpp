#include <iostream>
#include <vector>
#include <Windows.h>
#include "CfgSwitchAPI.h"
#include "tinyxml2.h"
#include "game.h"

std::vector<game> games;
std::string path;

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

	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* pathElement = rootNode->FirstChildElement("path");
	path = pathElement->GetText();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");
	while (gameElement != nullptr) {
		tinyxml2::XMLElement* element = gameElement->FirstChildElement("id");
		std::string gameID = element->GetText();
		element = gameElement->FirstChildElement("path");
		std::string gamePath = element->GetText();
		game tmpGame(gameID, gamePath, "", "");
		games.push_back(tmpGame);
		gameElement = gameElement->NextSiblingElement("game");
	}

	/*DWORD cfgFa = GetFileAttributesA("D:\\cfg-switcher\\cfg-switcher\\configs");
	if ((cfgFa == INVALID_FILE_ATTRIBUTES) || !(cfgFa & FILE_ATTRIBUTE_DIRECTORY)) {
		std::cout << "NO CONFIG FOLDER!" << std::endl;
		if (!createFileStruct()) {
			std::cerr << "Error creating config file directory structure..." << std::endl;
			return false;
		}
	}*/

	return true;
}

bool createSettingsFile() {
	tinyxml2::XMLDocument settings;
	tinyxml2::XMLNode* rootNode = settings.NewElement("settings");
	settings.InsertFirstChild(rootNode);
	tinyxml2::XMLElement* element = settings.NewElement("version");
	element->SetText(0.1f);
	rootNode->InsertEndChild(element);
	element = settings.NewElement("path");
	element->SetText("D:\\cfg-switcher\\cfg-switcher");
	rootNode->InsertEndChild(element);
	element = settings.NewElement("games");

	// TESTING

	tinyxml2::XMLElement* gamesElement = settings.NewElement("game");
	tinyxml2::XMLElement* gameElement = settings.NewElement("id");
	gameElement->SetText("GTA V");
	gamesElement->InsertEndChild(gameElement);
	gameElement = settings.NewElement("path");
	gameElement->SetText("C:\\Users\\unkno\\Documents\\Rockstar Games\\GTA V");
	gamesElement->InsertEndChild(gameElement);
	element->InsertEndChild(gamesElement);

	gamesElement = settings.NewElement("game");
	gameElement = settings.NewElement("id");
	gameElement->SetText("Borderlands");
	gamesElement->InsertEndChild(gameElement);
	gameElement = settings.NewElement("path");
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

bool createFileStruct() {
	return false;
}

bool initGames() {
	return false;
}

bool switchCfgs(powerState) {
	return false;
}
