#define NOMINMAX

#include <iostream>
#include <string>
#include <Windows.h>
#include <process.h>
#include <future>
#include <vector>
#include "WinUtils.h"
#include "PowerWindowThread.h"
#include "ConfigSwitchThread.h"
#include "CfgSwitchAPI.h"
#include "Settings.h"
#include "game.h"

const int NUM_THREAD_HANDLES = 2;
const int NUM_EVENT_HANDLES = 2;
const int NUM_HANDLES = NUM_THREAD_HANDLES + NUM_EVENT_HANDLES;
const int TIMEOUT = 5000;

int main() {
	Settings settings;

	// Attempt to initialize settings
	if (!settings.initSettings())
		return EXIT_FAILURE;

	std::vector<game>& games = settings.getGames();
	if (games.empty()) {
		std::cout << "*********************************" << std::endl;
		std::cout << "* NO GAMES CURRENTLY CONFIGURED *" << std::endl;
		std::cout << "*********************************" << std::endl;
	}

	// Initialize power and close events
	HANDLE PowerEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("PowerEvent"));
	HANDLE CloseEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("CloseEvent"));
	if (!PowerEvent || !CloseEvent) {
		CloseHandle(PowerEvent);
		CloseHandle(CloseEvent);
		std::cerr << "Error creating event handles: " + GetLastErrorAsString() << std::endl;
		return EXIT_FAILURE;
	}

	// Define message window handle promise/future
	std::promise<HWND> pwhPromise;
	std::future<HWND> pwhFuture = pwhPromise.get_future();

	// Initialize message window and config switch threads
	HANDLE WindowThread = (HANDLE)_beginthreadex(0, 0, &windowsPowerThread, (void *)&pwhPromise, 0, 0);
	HANDLE SwitchThread = (HANDLE)_beginthreadex(0, 0, &configSwitchThread, (void *)&settings, 0, 0);
	HANDLE ThreadHandles[NUM_THREAD_HANDLES] = { WindowThread, SwitchThread };
	for (HANDLE &th : ThreadHandles) {
		if (!th) {
			CloseHandle(ThreadHandles[0]);
			CloseHandle(ThreadHandles[1]);
			std::cerr << "Error creating threads: " + GetLastErrorAsString() << std::endl;
			return EXIT_FAILURE;
		}
	}

	// Attempt to retrieve handle to message window
	if (pwhFuture.wait_for(std::chrono::milliseconds(TIMEOUT)) != std::future_status::ready) {
		std::cerr << "Error getting handle to hidden power window..." << std::endl;
		return EXIT_FAILURE;
	}

	// Aggregate process handles
	HANDLE ProcHandles[NUM_HANDLES] = {PowerEvent, CloseEvent, WindowThread, SwitchThread};

	std::cout << "=====================================" << std::endl;
	std::cout << "Config Switcher successfully started!" << std::endl;
	std::cout << "=====================================" << std::endl;

	// Check for unset games
	std::vector<game> unset = settings.unsetGames();
	if (!unset.empty()) {
		std::cout << "Warning: The following games have unset config files" << std::endl;
		for (game &g : unset)
			std::cout << " - " << g.ID << std::endl;
	}

	std::vector<std::string> options = {
		"ADD GAME",
		"REMOVE GAME",
		"LIST GAMES",
		"SET CONFIGS",
		"EXIT"
	};

	int opt = 0;
	bool cont = true;

	while (cont) {
		std::cout << std::endl << "CHOOSE AN OPTION" << std::endl;
		std::cout << "----------------" << std::endl;
		for (int optInd = 0; optInd < options.size(); optInd++)
			std::cout << optInd + 1 << ":  " << options[optInd] << std::endl;
		std::cout << "- ";

		while (!(std::cin >> opt)) {
			std::cerr << "Error: Invalid input; expecting option number" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		std::string gameID;
		std::string cfgPath;
		int index;
		bool result;
		switch (opt) {
		case 1: // Add game
			std::cout << std::endl << "========" << std::endl;
			std::cout << "ADD GAME" << std::endl;
			std::cout << "========" << std::endl;
			std::cout << "Leave a field blank to cancel" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Enter game name: ";
			getline(std::cin, gameID);
			if (settings.gameExists(gameID)) {
				std::cerr << "Error: " << gameID << " already exists in configuration" << std::endl;
				break;
			}
			else if (gameID.empty() || (gameID.find_first_not_of(' ') == std::string::npos))
				break;
			std::cout << "Select directory containing the " << gameID << " config files..." << std::endl;
			cfgPath = BrowseFile("Select directory containing the " + gameID + " config files...");
			if (settings.addGame(gameID, cfgPath)) {
				std::cout << std::endl << "Successfully added " << gameID << " to configuration" << std::endl;
			}
			break;
		case 2: // Remove game
			std::cout << std::endl << "===========" << std::endl;
			std::cout << "REMOVE GAME" << std::endl;
			std::cout << "===========" << std::endl;
			index = 1;
			if (games.empty()) {
				std::cout << std::endl << "*********************************" << std::endl;
				std::cout << "* NO GAMES CURRENTLY CONFIGURED *" << std::endl;
				std::cout << "*********************************" << std::endl;
			}
			else {
				std::cout << "Which game would you like to remove?" << std::endl;
				for (game &g : games) {
					std::cout << index++ << ": " << g.ID << std::endl;
				}
				std::cout << index << ": CANCEL" << std::endl;
				opt = 0;
				while (opt < 1 || opt > games.size() + 1) {
					while (!(std::cin >> opt)) {
						std::cerr << "Error: Invalid input; expecting option number" << std::endl;
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
					if (opt < 1 || opt > games.size() + 1)
						std::cerr << "Error: Invalid option" << std::endl;
				}
				if (opt == games.size() + 1)
					break;
				game remGame = games.at(opt - 1);
				gameID = remGame.ID;

				if (!remGame.battCfgSet || !remGame.mainCfgSet) {
					if (settings.removeGame(remGame, NONE)) {
						std::cout << "Successfully removed " << gameID << "!" << std::endl;
						break;
					}
				}

				std::cout << "Which config would you like to apply before removal?" << std::endl;
				std::cout << "1: BATTERY\n2: MAIN\n3: NEITHER" << std::endl;
				opt = 0;
				while (opt < 1 || opt > 3) {
					while (!(std::cin >> opt)) {
						std::cerr << "Error: Invalid input; expecting option number" << std::endl;
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
					if (opt < 1 || opt > 3)
						std::cerr << "Error: Invalid option" << std::endl;
				}

				if (settings.removeGame(remGame, powerState(opt - 1))) {
					std::cout << "Successfully removed " << gameID << "!" << std::endl;
				}
			}
				break;
		case 3: // List games
			std::cout << std::endl << "=============" << std::endl;
			std::cout << "CURRENT GAMES" << std::endl;
			std::cout << "=============" << std::endl;
			if (games.empty()) {
				std::cout << std::endl << "*********************************" << std::endl;
				std::cout << "* NO GAMES CURRENTLY CONFIGURED *" << std::endl;
				std::cout << "*********************************" << std::endl;
			}
			else for (game &g : games) {
				std::cout << " - " << g.ID << std::endl;
				std::cout << "\tPath: " << g.cfgPath << std::endl;
				std::cout << "\tMain Config [" << (g.mainCfgSet ? "X" : " ") << "]" << std::endl;
				std::cout << "\tBatt Config [" << (g.battCfgSet ? "X" : " ") << "]" << std::endl;
			}
			break;
		case 4: // Set configs
			if (games.empty()) {
				std::cout << std::endl << "*********************************" << std::endl;
				std::cout << "* NO GAMES CURRENTLY CONFIGURED *" << std::endl;
				std::cout << "*********************************" << std::endl;
			}
			else {
				std::cout << "Set configs for MAIN or BATTERY power?" << std::endl;
				std::cout << "1: BATTERY\n2: MAIN\n3: CANCEL" << std::endl;
				opt = 0;
				while (opt < 1 || opt > 3) {
					while (!(std::cin >> opt)) {
						std::cerr << "Error: Invalid input; expecting option number" << std::endl;
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
					if (opt < 1 || opt > 3)
						std::cerr << "Error: Invalid option" << std::endl;
				}
				bool result;
				switch (opt) {
					case 1:
						result = settings.setConfigs(BATTERY);
						break;
					case 2:
						result = settings.setConfigs(MAIN);
						break;
					default:
						result = false;
						break;
				}
				if (result) {
					std::cout << std::endl << "=============================" << std::endl;
					std::cout << "CONFIG FILES SUCCESSFULLY SET" << std::endl;
					std::cout << "=============================" << std::endl;
				}
			}
			break;
		case 5: // Exit
			cont = false;
			break;
		default:
			std::cerr << "Error: Invalid option" << std::endl;
		}
	}

	// Destroy message window to terminate thread
	SendMessage(pwhFuture.get(), WM_DESTROY, NULL, NULL);

	// Wait for threads to terminate
	DWORD threadStatus = WaitForMultipleObjects(NUM_THREAD_HANDLES, ThreadHandles, TRUE, INFINITE);
	
	// Close handles
	for (HANDLE &th : ThreadHandles)
		CloseHandle(th);

	// Verify threads terminated successfully
	switch (threadStatus) {
		case WAIT_OBJECT_0:
			printf("All threads ended, cleaning up for application exit...\n");
			break;
		default:
			printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
			return 1;
	}

	// DEBUG
	//settings.deleteSettingsFile();

	return EXIT_SUCCESS;
}
