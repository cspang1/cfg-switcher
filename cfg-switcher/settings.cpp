#include <Windows.h>
#include <QMessageBox>
#include <QDir>
#include "tinyxml2.h"
#include "game.h"
#include "settings.h"

Settings::Settings() {
    path = QDir::currentPath();
	cfgPath = path + "\\configs";
    settingsPath = path + "\\settings.xml";
}

bool Settings::initSettings() {
    QMessageBox msg;
	tinyxml2::XMLDocument settings;
    tinyxml2::XMLError loaded = settings.LoadFile(settingsPath.toStdString().c_str());
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
    QString gameID;
    QString gamePath;
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
        games.push_back(Game(gameID, gamePath, mainCfgSet, battCfgSet));
		gameElement = gameElement->NextSiblingElement("game");
	}

    if(!QDir(cfgPath).exists()) {
        msg.setText(cfgPath + " doesn't exist!");
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
    element->SetText(path.toStdString().c_str());
	rootNode->InsertEndChild(element);
	rootNode->InsertEndChild(settings.NewElement("games"));
	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS)
		return false;
	return true;
}

bool Settings::createFileStruct() {
    QMessageBox msg;
    if(!QDir().mkdir(cfgPath)) {
        msg.setText("Error creating " + cfgPath);
        msg.exec();
        return false;
    }

    return updateFileStruct();
}

bool Settings::updateFileStruct() {
    QMessageBox msg;
    QString gamePath;
    QString tempPath;
    QDir gpdCreate;
    QDir gmdCreate;
    QDir gbdCreate;
    for (Game &g : games) {
        gamePath = cfgPath + "\\" + g.ID;
        gpdCreate.mkpath(gamePath);
        gpdCreate.setPath(gamePath);
        tempPath = gamePath + "\\main";
        gmdCreate.mkpath(tempPath);
        gmdCreate.setPath(tempPath);
        tempPath = gamePath + "\\battery";
        gbdCreate.mkpath(tempPath);
        gbdCreate.setPath(tempPath);
        if (!gpdCreate.exists() || !gmdCreate.exists() || !gbdCreate.exists()) {
            msg.setText("Error creating dirs?");
            msg.exec();
		}
    }

	return true;
}

bool Settings::addGame(QString gameID, QString gameCfgPath) {
    QMessageBox msg;
    if(gameExists(gameID)) {
        msg.setText(gameID + " already exists in configuration");
        msg.exec();
        return false;
    }
	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
        msg.setText("Error: Unable to load settings.xml file");
        msg.exec();
        return false;
	}
	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = settings.NewElement("game");
	tinyxml2::XMLElement* element = settings.NewElement("id");
    element->SetText(gameID.toStdString().c_str());
	gameElement->InsertEndChild(element);
	element = settings.NewElement("path");
    if (gameCfgPath.isEmpty()) {
        msg.setText("Error: Valid game config path required");
        msg.exec();
        return false;
	}
    element->SetText(gameCfgPath.toStdString().c_str());
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
        msg.setText("Error: Unable to save settings.xml file");
        msg.exec();
        return false;
	}

    games.push_back(Game(gameID, gameCfgPath, false, false));

	return updateFileStruct();
}

bool Settings::removeGame(QString gameID) {
    QMessageBox msg;

	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
        msg.setText("Error: Unable to load settings.xml file");
        msg.exec();
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
                msg.setText("Error: Unable to save settings.xml file");
                msg.exec();
                return false;
			}
			else
				break;
		}
		gameElement = gameElement->NextSiblingElement("game");
	}

    if(!QDir(cfgPath + "\\" + gameID).removeRecursively()) {
        msg.setText("Error: Unable to delete game config directory");
        msg.exec();
        return false;
    }

    for (int index = 0; index < games.size(); index++) {
        if (!gameID.compare(games.at(index).ID)) {
            games.erase(games.begin() + index);
		}
	}

	return true;
}

bool Settings::gameExists(QString gameID) {
    for (Game &g : games) {
		if (!(gameID.compare(g.ID)))
			return true;
	}

	return false;
}

bool Settings::setConfigs(int tgtState) {
    QMessageBox msg;
    QString cfgDest;
    QString cfgFile;

	tinyxml2::XMLDocument settings;
	tinyxml2::XMLError loaded = settings.LoadFile("settings.xml");
	if (loaded != tinyxml2::XML_SUCCESS) {
        msg.setText("Error: Unable to load settings.xml file");
        msg.exec();
        return false;
	}
	tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");

    for (Game &g : games) {
        QFileInfo fileInfo(QFile(g.cfgPath).fileName());
        cfgFile = fileInfo.fileName();
		switch (tgtState) {
            case 0:
                cfgDest = cfgPath + "\\" + g.ID + "\\battery\\" + cfgFile;
                break;
            case 1:
				cfgDest = cfgPath + "\\" + g.ID + "\\main\\" + cfgFile;
				break;
			default:
                msg.setText("Error: Invalid AClineStatus provided");
                msg.exec();
				return false;
		}

        if(QFile::exists(cfgDest))
            QFile::remove(cfgDest);
        if(!QFile::copy(g.cfgPath, cfgDest)) {
            msg.setText("Error: Unable to copy config file");
            msg.exec();
            return false;
        }
		else {
			while (g.ID.compare(gameElement->FirstChildElement("id")->GetText()))
				gameElement = gameElement->NextSiblingElement("game");

			switch (tgtState) {
                case 0:
                    g.battCfgSet = true;
                    gameElement->FirstChildElement("battcfgset")->SetAttribute("value", g.battCfgSet);
                    break;
                case 1:
					g.mainCfgSet = true;
					gameElement->FirstChildElement("maincfgset")->SetAttribute("value", g.mainCfgSet);
					break;
				default:
                    msg.setText("Error: Invalid AClineStatus provided");
                    msg.exec();
                    return false;
			}
		}
    }

	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS) {
        msg.setText("Error: Unable to save settings.xml file");
        msg.exec();
        return false;
	}

	return true;
}

QList<Game> Settings::unsetGames() {
    QList<Game> unset;
    for (Game &g : games)
		if (!g.battCfgSet || !g.mainCfgSet)
			unset.push_back(g);

	return unset;
}
