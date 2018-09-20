#include <iostream>
#include <vector>
#include <Windows.h>
#include "WinUtils.h"
#include "CfgSwitchAPI.h"
#include "tinyxml2.h"
#include "game.h"
#include "Settings.h"

void Settings::deleteSettingsFile() {
	remove("settings.xml");
}

bool Settings::initSettings() {
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
	cfgPath = path + "\\configs";
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");
	std::string gameID;
	std::string gamePath;
	bool mainCfgSet;
	bool battCfgSet;
	tinyxml2::XMLElement* element;
	while (gameElement != nullptr) {
		element = gameElement->FirstChildElement("id");
		gameID = element->GetText();
		element = gameElement->FirstChildElement("path");
		gamePath = element->GetText();
		element = gameElement->FirstChildElement("maincfgset");
		mainCfgSet = element->BoolAttribute("value");
		element = gameElement->FirstChildElement("battcfgset");
		battCfgSet = element->BoolAttribute("value");
		cfgsSet(); // PERFORM CHECK FOR MAIN/BATT SETTINGS FILES HERE
		games.push_back(game(gameID, gamePath, mainCfgSet, battCfgSet));
		gameElement = gameElement->NextSiblingElement("game");
	}

	DWORD cfgFa = GetFileAttributesA(std::string(path + "\\configs").c_str());
	if ((cfgFa == INVALID_FILE_ATTRIBUTES) || !(cfgFa & FILE_ATTRIBUTE_DIRECTORY)) {
		if (!createFileStruct())
			return false;
	}

	return true;
}

bool Settings::createSettingsFile() {
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

bool Settings::createFileStruct() {
	if (!CreateDirectory(cfgPath.c_str(), NULL)) {
		std::cerr << "Error creating configs directory: " + GetLastErrorAsString() << std::endl;
		return false;
	}

	return updateFileStruct();
}

bool Settings::updateFileStruct() {
	for (game &g : games) {
		std::string gamePath(cfgPath + "\\" + g.ID);
		bool gpdCreate = CreateDirectory(gamePath.c_str(), NULL);
		bool gmdCreate = CreateDirectory(std::string(gamePath + "\\main").c_str(), NULL);
		bool gbdCreate = CreateDirectory(std::string(gamePath + "\\battery").c_str(), NULL);
		if (!gpdCreate || !gmdCreate || !gbdCreate) {
			if (GetLastError() != ERROR_ALREADY_EXISTS) {
				std::cerr << "Error creating games directories: " + GetLastErrorAsString() << std::endl;
				return false;
			}
		}
	}

	return true;
}

bool Settings::cfgsSet() {
	return false;
}

bool Settings::addGame(std::string gameID, std::string gameCfgPath) {
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
	if (gameCfgPath.empty()) {
		std::cerr << "Error: Valid file path required" << std::endl;
		return false;
	}
	element->SetText(gameCfgPath.c_str());
	gameElement->InsertEndChild(element);
	element = settings.NewElement("maincfgset");
	element->SetAttribute("value", false);
	gameElement->InsertEndChild(element);
	element = settings.NewElement("battcfgset");
	element->SetAttribute("value", false);
	gameElement->InsertEndChild(element);
	gamesElement->InsertEndChild(gameElement);
	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error: Unable to add game to configuration" << std::endl;
		return false;
	}

	games.push_back(game(gameID, gameCfgPath, false, false));

	return updateFileStruct();
}

bool Settings::gameExists(std::string gameID) {
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

bool Settings::setConfigs(powerState tgtState) {
	std::string cfgDest;
	std::string cfgFile;

	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error loading settings file" << std::endl;
		return false;
	}
	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");
	tinyxml2::XMLElement* element;

	for (game &g : games) {
		cfgFile = FileFromPath(g.cfgPath);
		switch (tgtState) {
			case MAIN:
				cfgDest = cfgPath + "\\" + g.ID + "\\main\\" + cfgFile;
				break;
			case BATTERY:
				cfgDest = cfgPath + "\\" + g.ID + "\\battery\\" + cfgFile;
				break;
			default:
				std::cerr << "Error: Invalid AC line state specified" << std::endl;
				return false;
		}

		if (!copyFile(g.cfgPath.c_str(), cfgDest.c_str())) {
			std::cerr << "Error: Unable to copy config file" << std::endl;
			return false;
		}
		else {
			while (g.ID.compare(gameElement->FirstChildElement("id")->GetText()))
				gameElement = gameElement->NextSiblingElement("game");

			switch (tgtState) {
				case MAIN:
					g.mainCfgSet = true;
					gameElement->FirstChildElement("maincfgset")->SetAttribute("value", g.mainCfgSet);
					break;
				case BATTERY:
					g.battCfgSet = true;
					gameElement->FirstChildElement("battcfgset")->SetAttribute("value", g.battCfgSet);
					break;
				default:
					std::cerr << "Error: Invalid AC line state specified" << std::endl;
					return false;
			}
		}
	}

	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error: Unable to update game configuration" << std::endl;
		return false;
	}

	return true;
}

std::vector<game> Settings::unsetGames() {
	std::vector<game> unset;
	for (game &g : games)
		if (!g.battCfgSet || !g.mainCfgSet)
			unset.push_back(g);

	return unset;
}

std::vector<game> Settings::getGames() {
	return games;
}
