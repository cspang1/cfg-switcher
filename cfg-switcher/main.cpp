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

	std::vector<game> games = settings.getGames();
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
	HANDLE SwitchThread = (HANDLE)_beginthreadex(0, 0, &configSwitchThread, (void *)&games, 0, 0);
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

	std::vector<std::string> options = {
		"ADD GAME",
		"EXIT"
	};

	int opt = 0;
	bool cont = true;
	bool restReq = false;

	while (cont) {
		std::cout << std::endl << "CHOOSE AN OPTION" << std::endl;
		std::cout << "----------------" << std::endl;
		for (int optInd = 0; optInd < options.size(); optInd++)
			std::cout << optInd + 1 << ":  " << options[optInd] << std::endl;
		if (restReq) {
			std::cout << "********************" << std::endl;
			std::cout << "* RESTART REQUIRED *" << std::endl;
			std::cout << "********************" << std::endl;
		}
		std::cout << "- ";

		while (!(std::cin >> opt)) {
			std::cerr << "Error: Invalid input; expecting option number" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		std::string gameID;
		switch (opt) {
		case 1: // Add game
			std::cout << std::endl << "========" << std::endl;
			std::cout << "ADD GAME" << std::endl;
			std::cout << "========" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Enter game name: ";
			getline(std::cin, gameID);
			std::cout << "Select directory containing the " << gameID << " config files..." << std::endl;
			if (settings.addGame(gameID)) {
				std::cout << std::endl << "===================================================" << std::endl;
				std::cout << "Successfully added " << gameID << " to configuration!" << std::endl;
				std::cout << "Restart Config Switcher for changes to take effect" << std::endl;
				std::cout << "===================================================" << std::endl;
				restReq = true;
			}
			break;
		case 2: // Exit
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
	//deleteSettingsFile();

	return EXIT_SUCCESS;
}
