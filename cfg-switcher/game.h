#ifndef GAME_H
#define GAME_H

#include <QString>
#include <QMetaType>
#include <QDataStream>

class Game {
public:
    QString ID;
    QString cfgPath;
    bool mainCfgSet;
    bool battCfgSet;
    bool enabled;
    Game& operator=(const Game& other) {
        ID = other.ID;
        cfgPath = other.cfgPath;
        mainCfgSet = other.mainCfgSet;
        battCfgSet = other.battCfgSet;
        enabled = other.enabled;
        return *this;
    }
    Game() :
        ID(""), cfgPath(""), mainCfgSet(false), battCfgSet(false), enabled(false) {}
    Game(const Game& other) :
        ID(other.ID), cfgPath(other.cfgPath), mainCfgSet(other.mainCfgSet), battCfgSet(other.battCfgSet), enabled(other.enabled) {}
    Game(QString _ID, QString _cfgPath, bool _mainCfgSet = false, bool _battCfgSet = false, bool _enabled = false) :
        ID(_ID), cfgPath(_cfgPath), mainCfgSet(_mainCfgSet), battCfgSet(_battCfgSet), enabled(_enabled) {}

private:
    friend QDataStream& operator << (QDataStream& out, const Game& game) {
        out << game.ID << game.cfgPath << game.mainCfgSet << game.battCfgSet << game.enabled;
        return out;
    }
    friend QDataStream& operator >> (QDataStream& in, Game& game) {
        in >> game.ID >> game.cfgPath >> game.mainCfgSet >> game.battCfgSet >> game.enabled;
        return in;
    }
};

Q_DECLARE_METATYPE(Game);

#endif // GAME_H
