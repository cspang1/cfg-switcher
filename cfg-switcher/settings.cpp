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

QList<Game> Settings::initSettings() {
    QMessageBox msg;
	tinyxml2::XMLDocument settings;
    tinyxml2::XMLError loaded = settings.LoadFile(settingsPath.toStdString().c_str());
    status = true;
    if (loaded != tinyxml2::XML_SUCCESS) {
		if (!createSettingsFile()) {
            msg.setText("Error creating new settings file");
            msg.exec();
            status = false;
            return QList<Game>();
		}
		else {
			loaded = settings.LoadFile("settings.xml");
			if (loaded != tinyxml2::XML_SUCCESS) {
                msg.setText("Error loading settings file");
                msg.exec();
                status = false;
			}
		}
	}

    QList<Game> games;
    tinyxml2::XMLNode* rootNode = settings.FirstChild();
	tinyxml2::XMLElement* pathElement = rootNode->FirstChildElement("path");
	path = pathElement->GetText();
	cfgPath = path + "\\configs";
    settingsPath = path + "\\settings.xml";
	tinyxml2::XMLElement* gamesElement = rootNode->FirstChildElement("games");
	tinyxml2::XMLElement* gameElement = gamesElement->FirstChildElement("game");

	while (gameElement != nullptr) {
        tinyxml2::XMLElement* element = gameElement->FirstChildElement("id");
        QString gameID = element->GetText();
		element = gameElement->FirstChildElement("path");
        QString gamePath = element->GetText();
		element = gameElement->FirstChildElement("maincfgset");
        bool mainCfgSet = element->BoolAttribute("value");
        element = gameElement->FirstChildElement("battcfgset");
        bool battCfgSet = element->BoolAttribute("value");
        element = gameElement->FirstChildElement("enabled");
        bool enabled = element->BoolAttribute("value");
        games.push_back(Game(gameID, gamePath, mainCfgSet, battCfgSet, enabled));
		gameElement = gameElement->NextSiblingElement("game");
	}

    if(!QDir(cfgPath).exists()) {
        if(!createFileStruct(games)) {
            status = false;
            return QList<Game>();
        }
    }

    return games;
}

bool Settings::createSettingsFile() {
	tinyxml2::XMLDocument settings;
	tinyxml2::XMLNode* rootNode = settings.NewElement("settings");
	settings.InsertFirstChild(rootNode);
	tinyxml2::XMLElement* element = settings.NewElement("version");
    element->SetText("2.0.0");
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

bool Settings::createFileStruct(QList<Game> games) {
    QMessageBox msg;
    if(!QDir().mkdir(cfgPath)) {
        msg.setText("Error creating " + cfgPath);
        msg.exec();
        return false;
    }

    bool success = true;
    for (Game &g : games)
        if(!updateFileStruct(g.ID))
            success = false;

    return success;
}

bool Settings::updateFileStruct(QString gameID) {
    QDir gpdCreate;
    QDir gmdCreate;
    QDir gbdCreate;
    QString gamePath = cfgPath + "\\" + gameID;
    gpdCreate.mkpath(gamePath);
    gpdCreate.setPath(gamePath);
    QString tempPath = gamePath + "\\main";
    gmdCreate.mkpath(tempPath);
    gmdCreate.setPath(tempPath);
    tempPath = gamePath + "\\battery";
    gbdCreate.mkpath(tempPath);
    gbdCreate.setPath(tempPath);
    if (!gpdCreate.exists() || !gmdCreate.exists() || !gbdCreate.exists()) {
        QMessageBox msg;
        msg.setText("Error creating " + gameID + "config directories");
        msg.exec();
        return false;
    }

	return true;
}

bool Settings::addGame(QString gameID, QString gameCfgPath) {
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
    element = settings.NewElement("enabled");
    element->SetAttribute("value", false);
    gameElement->InsertEndChild(element);
	gamesElement->InsertEndChild(gameElement);
	tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
	if (saved != tinyxml2::XML_SUCCESS) {
        msg.setText("Error: Unable to save settings.xml file");
        msg.exec();
        return false;
	}

    return updateFileStruct(gameID);
}

bool Settings::removeGame(QString gameID) {
    QMessageBox msg;

    // Delete game from settings file
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

    // Delete game config directory
    if(!QDir(cfgPath + "\\" + gameID).removeRecursively()) {
        msg.setText("Error: Unable to delete game config directory");
        msg.exec();
        return false;
    }

	return true;
}

// Re-implement to check if explicitly exists in settings file?
/*bool Settings::gameExists(QString gameID) {
    for (Game &g : games) {
		if (!(gameID.compare(g.ID)))
			return true;
	}

	return false;
}*/

bool Settings::setConfig(int tgtState, Game game) {
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

    QFileInfo fileInfo(QFile(game.cfgPath).fileName());
    cfgFile = fileInfo.fileName();
    switch (tgtState) {
        case 0:
            cfgDest = cfgPath + "\\" + game.ID + "\\battery\\" + cfgFile;
            break;
        case 1:
            cfgDest = cfgPath + "\\" + game.ID + "\\main\\" + cfgFile;
            break;
        default:
            msg.setText("Error: Invalid AClineStatus provided");
            msg.exec();
            return false;
    }

    if(QFile::exists(cfgDest))
        QFile::remove(cfgDest);
    if(!QFile::copy(game.cfgPath, cfgDest)) {
        msg.setText("Error: Unable to copy config file");
        msg.exec();
        return false;
    }
    else {
        while (game.ID.compare(gameElement->FirstChildElement("id")->GetText()))
            gameElement = gameElement->NextSiblingElement("game");

        switch (tgtState) {
            case 0:
                game.battCfgSet = true;
                gameElement->FirstChildElement("battcfgset")->SetAttribute("value", game.battCfgSet);
                break;
            case 1:
                game.mainCfgSet = true;
                gameElement->FirstChildElement("maincfgset")->SetAttribute("value", game.mainCfgSet);
                break;
            default:
                msg.setText("Error: Invalid AClineStatus provided");
                msg.exec();
                return false;
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

bool Settings::setStatus(bool enabled, Game game) {
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

    while (game.ID.compare(gameElement->FirstChildElement("id")->GetText()))
        gameElement = gameElement->NextSiblingElement("game");

    game.enabled = enabled;
    gameElement->FirstChildElement("enabled")->SetAttribute("value", enabled);

    tinyxml2::XMLError saved = settings.SaveFile("settings.xml");
    if (saved != tinyxml2::XML_SUCCESS) {
        msg.setText("Error: Unable to save settings.xml file");
        msg.exec();
        return false;
    }

    return true;
}
