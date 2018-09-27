#ifndef GAME_H
#define GAME_H

#include <QString>
#include <QMetaType>

class Game {
public:
    QString ID;
    QString cfgPath;
	bool mainCfgSet;
	bool battCfgSet;
    Game& operator=(const Game& other) {
		ID = other.ID;
		cfgPath = other.cfgPath;
		mainCfgSet = other.mainCfgSet;
		battCfgSet = other.battCfgSet;
		return *this;
	}
    Game(const Game& other) {
        ID = other.ID;
        cfgPath = other.cfgPath;
        mainCfgSet = other.mainCfgSet;
        battCfgSet = other.battCfgSet;
    }
    Game() : ID(""), cfgPath(""), mainCfgSet(false), battCfgSet(false) {}
    Game(QString _ID, QString _cfgPath, bool _mainCfgSet = false, bool _battCfgSet = false)
        :ID(_ID), cfgPath(_cfgPath), mainCfgSet(_mainCfgSet), battCfgSet(_battCfgSet) { }
};

Q_DECLARE_METATYPE(Game);

#endif // GAME_H
