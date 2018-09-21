#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include "ConfigSwitchThread.h"
#include "CfgSwitchAPI.h"
#include "WinUtils.h"
#include "game.h"

const int NUM_HANDLES = 2;
BYTE CurrentACStatus;

unsigned int __stdcall configSwitchThread(void* data) {
	std::vector<game>& games = *(std::vector<game>*)data;

	HANDLE EventHandles[NUM_HANDLES];
	EventHandles[0] = OpenEvent(EVENT_ALL_ACCESS, false, TEXT("PowerEvent"));
	EventHandles[1] = OpenEvent(EVENT_ALL_ACCESS, false, TEXT("CloseEvent"));

	BYTE ACLineStatus = { 0 };
	bool ACStatusChanged = { 0 };

	// Perform initial config switch
	ACLineStatus = getPowerStatus();
	CurrentACStatus = ACLineStatus;
	ACStatusChanged = false;
	switch (CurrentACStatus) {
		case 0:
			switchConfigs(BATTERY);
			break;
		case 1:
			switchConfigs(MAIN);
			break;
		default:
			std::cerr << "Error: Invalid AC line status - " << GetLastErrorAsString() << std::endl;
	}

	while (true) {
		DWORD objStat = WaitForMultipleObjects(NUM_HANDLES, EventHandles, FALSE, INFINITE);
		switch (objStat) {
			case WAIT_OBJECT_0:
				ACLineStatus = getPowerStatus();
				ACStatusChanged = CurrentACStatus != ACLineStatus;
				CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
				if (ACStatusChanged) {
					switch (CurrentACStatus) {
						case 0:
							switchConfigs(BATTERY);
							break;
						case 1:
							switchConfigs(MAIN);
							break;
						default:
							std::cerr << "Error: Invalid AC line status - " << GetLastErrorAsString() << std::endl;
					}
					SetEvent(EventHandles[0]);
				}
				ResetEvent(EventHandles[0]);
				break;
			case WAIT_OBJECT_0 + 1:
				ResetEvent(EventHandles[1]);
				for (HANDLE& handle : EventHandles)
					CloseHandle(handle);
				return EXIT_SUCCESS;
				break;
			default:
				std::cerr << "Error: " << GetLastErrorAsString() << std::endl;
				return EXIT_FAILURE;
		}
	}
	
	for (HANDLE& handle : EventHandles)
		CloseHandle(handle);

	return EXIT_SUCCESS;
}

BYTE getPowerStatus() {
	SYSTEM_POWER_STATUS lpSystemPowerStatus;
	if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
		std::string errMsg = "Error getting system power status: " + GetLastErrorAsString();
		std::cerr << errMsg << std::endl;
		return EXIT_FAILURE;
	}

	return lpSystemPowerStatus.ACLineStatus;
}

bool switchConfigs(powerState pState) {
	std::cout << "SWITCHING CONFIGURATION FILES TO " + std::string(pState ? "PLUGGED IN" : "UNPLUGGED") << std::endl;
	return true;
}