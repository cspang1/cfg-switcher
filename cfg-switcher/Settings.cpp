#include <iostream>
#include <vector>
#include <Windows.h>
#include "WinUtils.h"
#include "CfgSwitchAPI.h"
#include "tinyxml2.h"
#include "game.h"
#include "Settings.h"

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
			std::cerr << "Error creating new settings file" << std::endl;
			return false;
		}
		else {
			loaded = settings.LoadFile("settings.xml");
			if (loaded != tinyxml2::XML_SUCCESS) {
				std::cerr << "Error loading settings file" << std::endl;
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
		games.push_back(game(gameID, gamePath, "", ""));
		gameElement = gameElement->NextSiblingElement("game");
	}

	DWORD cfgFa = GetFileAttributesA(std::string(path + "\\configs").c_str());
	if ((cfgFa == INVALID_FILE_ATTRIBUTES) || !(cfgFa & FILE_ATTRIBUTE_DIRECTORY)) {
		if (!createFileStruct())
			return false;
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
	element = settings.NewElement("path");
	element->SetText("D:\\cfg-switcher\\cfg-switcher");
	rootNode->InsertEndChild(element);
	rootNode->InsertEndChild(settings.NewElement("games"));
	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS)
		return false;
	return true;
}

bool createFileStruct() {
	std::string cfgPath(path + "\\configs");
	if (!CreateDirectory(cfgPath.c_str(), NULL)) {
		std::cerr << "Error creating configs directory: " + GetLastErrorAsString() << std::endl;
		return false;
	}
	for (game &g : games) {
		std::string gamePath(cfgPath + "\\" + g.ID);
		bool gpdCreate = CreateDirectory(gamePath.c_str(), NULL);
		bool gmdCreate = CreateDirectory(std::string(gamePath + "\\main").c_str(), NULL);
		bool gbdCreate = CreateDirectory(std::string(gamePath + "\\battery").c_str(), NULL);
		if (!gpdCreate || !gmdCreate || !gbdCreate) {
			std::cerr << "Error creating games directories: " + GetLastErrorAsString() << std::endl;
			return false;
		}
	}

	return true;
}

bool addGame(std::string gameID) {
	if (gameExists(gameID)) {
		std::cerr << "Error: " << gameID << " already exists in configuration" << std::endl;
		return false;
	}

	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error loading settings file" << std::endl;
		return false;
	}
	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = settings.NewElement("game");
	tinyxml2::XMLElement* element = settings.NewElement("id");
	element->SetText(gameID.c_str());
	gameElement->InsertEndChild(element);
	element = settings.NewElement("path");
	std::string path = BrowseFolder("Select directory containing the " + gameID + " config files...");
	if (path.empty()) {
		std::cerr << "Error: Valid directory path required" << std::endl;
		return false;
	}
	element->SetText(path.c_str());
	gameElement->InsertEndChild(element);
	gamesElement->InsertEndChild(gameElement);
	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error: Unable to add game to configuration" << std::endl;
		return false;
	}

	return true;
}

bool gameExists(std::string gameID) {
	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error loading settings file" << std::endl;
		return false;
	}
	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");
	tinyxml2::XMLElement* idElement;
	while (gameElement != nullptr) {
		idElement = gameElement->FirstChildElement("id");
		if (!gameID.compare(idElement->GetText()))
			return true;
		gameElement = gameElement->NextSiblingElement("game");
	}

	return false;
}

std::vector<game> getGames() {
	return games;
}
