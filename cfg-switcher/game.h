#pragma once
#include <string>

class game {
	std::string ID;
	std::string cfgPath;
	std::string battCfg;
	std::string mainCfg;
public:
	game(std::string, std::string, std::string, std::string);
	bool setBattCfg();
	bool setMainCfg();
};