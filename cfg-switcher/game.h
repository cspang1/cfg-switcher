#pragma once
#include <string>

class game {
public:
	const std::string ID;
	const std::string cfgPath;
	bool mainCfgSet;
	bool battCfgSet;
	game(std::string _ID, std::string _cfgPath, bool _mainCfgSet, bool _battCfgSet)
		:ID(_ID), cfgPath(_cfgPath), mainCfgSet(_mainCfgSet), battCfgSet(_battCfgSet) { };
};