#pragma once
#include <string>

class game {
public:
	std::string ID;
	std::string cfgPath;
	bool mainCfgSet;
	bool battCfgSet;
	game& operator=(const game& other) {
		ID = other.ID;
		cfgPath = other.cfgPath;
		mainCfgSet = other.mainCfgSet;
		battCfgSet = other.battCfgSet;
		return *this;
	}
    game(std::string _ID, std::string _cfgPath, bool _mainCfgSet = false, bool _battCfgSet = false)
        :ID(_ID), cfgPath(_cfgPath), mainCfgSet(_mainCfgSet), battCfgSet(_battCfgSet) { }
};
