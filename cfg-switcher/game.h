#pragma once
#include <string>

class game {
	std::string _ID;
	std::string _cfgPath;
	std::string _battCfg;
	std::string _mainCfg;
	friend std::ostream& operator<<(std::ostream&, const game&);
public:
	game(std::string, std::string, std::string, std::string);
};