#include <vector>
#include <Windows.h>
#include <QMessageBox>
#include <QDir>
#include "CfgSwitchAPI.h"
#include "tinyxml2.h"
#include "game.h"
#include "Settings.h"

Settings::Settings() {
    path = QDir::currentPath().toStdString();
	cfgPath = path + "\\configs";
    settingsPath = path + "\\settings.xml";
}

bool Settings::initSettings() {
    QMessageBox msg;
	tinyxml2::XMLDocument settings;
    tinyxml2::XMLError loaded = settings.LoadFile(settingsPath.c_str());
	if (loaded != tinyxml2::XML_SUCCESS) {
        msg.setText("Couldn't find settings.xml file...");
        msg.exec();
		if (!createSettingsFile()) {
            msg.setText("Error creating new settings file");
            msg.exec();
			return false;
		}
		else {
			loaded = settings.LoadFile("settings.xml");
			if (loaded != tinyxml2::XML_SUCCESS) {
                msg.setText("Error loading settings file");
                msg.exec();
				return false;
			}
		}
	}

	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* pathElement = rootNode->FirstChildElement("path");
	path = pathElement->GetText();
	cfgPath = path + "\\configs";
    settingsPath = path + "\\settings.xml";
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
		games.push_back(game(gameID, gamePath, mainCfgSet, battCfgSet));
		gameElement = gameElement->NextSiblingElement("game");
	}

    if(!QDir(QString::fromStdString(cfgPath)).exists()) {
        msg.setText(QString::fromStdString(cfgPath) + " doesn't exist!");
        msg.exec();
        if(!createFileStruct())
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
	element->SetText(path.c_str());
	rootNode->InsertEndChild(element);
	rootNode->InsertEndChild(settings.NewElement("games"));
	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS)
		return false;
	return true;
}

bool Settings::createFileStruct() {
    QMessageBox msg;
    if(!QDir().mkdir(QString::fromStdString(cfgPath))) {
        msg.setText("Error creating " + QString::fromStdString(cfgPath));
        msg.exec();
        return false;
    }

    return updateFileStruct();
}

bool Settings::updateFileStruct() {
    QMessageBox msg;
    std::string gamePath;
    std::string tempPath;
    QDir gpdCreate;
    QDir gmdCreate;
    QDir gbdCreate;
    for (game &g : games) {
        gamePath = cfgPath + "\\" + g.ID;
        gpdCreate.mkpath(QString::fromStdString(gamePath));
        gpdCreate.setPath(QString::fromStdString(gamePath));
        tempPath = gamePath + "\\main";
        gmdCreate.mkpath(QString::fromStdString(tempPath));
        gmdCreate.setPath(QString::fromStdString(tempPath));
        tempPath = gamePath + "\\battery";
        gbdCreate.mkpath(QString::fromStdString(tempPath));
        gbdCreate.setPath(QString::fromStdString(tempPath));
        if (!gpdCreate.exists() || !gmdCreate.exists() || !gbdCreate.exists()) {
            msg.setText("Error creating dirs?");
            msg.exec();
		}
    }

	return true;
}

bool Settings::addGame(std::string gameID, std::string gameCfgPath) {
	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
        //std::cerr << "Error loading settings file" << std::endl;
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
        //std::cerr << "Error: Valid file path required" << std::endl;
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
        //std::cerr << "Error: Unable to add game to configuration" << std::endl;
		return false;
	}

	games.push_back(game(gameID, gameCfgPath, false, false));

	return updateFileStruct();
}

bool Settings::removeGame(game remGame, int keep) {
	std::string gameID = remGame.ID;
    if(keep != 2)
		switchConfigs(keep, *this, remGame);

	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
        //std::cerr << "Error loading settings file" << std::endl;
		return false;
	}
	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");
	tinyxml2::XMLElement* idElement;
	while (gameElement != nullptr) {
		idElement = gameElement->FirstChildElement("id");
		if (!gameID.compare(idElement->GetText())) {
			gamesElement->DeleteChild(gameElement);
			tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
			if (saved != tinyxml2::XML_SUCCESS) {
                //std::cerr << "Error: Unable to update game configuration" << std::endl;
				return false;
			}
			else
				break;
		}
		gameElement = gameElement->NextSiblingElement("game");
	}

    // Delete game directory here
    /*if (DeleteDirectory(std::string(cfgPath + "\\" + remGame.ID)))
        std::cerr << "Error: Unable to delete " << remGame.ID << " config directory" << std::endl;*/

	for (int index = 0; index < games.size(); index++) {
		if (!remGame.ID.compare(games.at(index).ID)) {
			games.erase(games.begin() + index);
		}
	}

	return true;
}

bool Settings::gameExists(std::string gameID) {
	for (game &g : games) {
		if (!(gameID.compare(g.ID)))
			return true;
	}

	return false;
}

bool Settings::setConfigs(int tgtState) {
	std::string cfgDest;
	std::string cfgFile;

	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
        //std::cerr << "Error loading settings file" << std::endl;
		return false;
	}
	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");

    // Set config files here
    /*
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
    }*/

	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS) {
        //std::cerr << "Error: Unable to update game configuration" << std::endl;
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
