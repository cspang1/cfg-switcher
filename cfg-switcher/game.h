#pragma once
#include <string>

class game {
public:
	const std::string ID;
	const std::string cfgPath;
	const std::string battCfg;
	const std::string mainCfg;
	game(std::string _ID, std::string _cfgPath, std::string _battCfg, std::string _mainCfg)
		:ID(_ID), cfgPath(_cfgPath), battCfg(_battCfg), mainCfg(_mainCfg) { };
};