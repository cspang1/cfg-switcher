#include "game.h"

game::game(std::string ID, std::string cfgPath, std::string battCfg, std::string mainCfg) {
	_ID = ID;
	_cfgPath = cfgPath;
	_battCfg = battCfg;
	_mainCfg = mainCfg;
}

std::ostream& operator<<(std::ostream &strm, const game &thisGame) {
	return strm << thisGame._ID << "'s config files are in " << thisGame._cfgPath;
}