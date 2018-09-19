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
	std::vector<game>& gees = *(std::vector<game>*)data;

	// Perform initial power status check
	CurrentACStatus = getPowerStatus();

	HANDLE EventHandles[NUM_HANDLES];
	EventHandles[0] = OpenEvent(EVENT_ALL_ACCESS, false, TEXT("PowerEvent"));
	EventHandles[1] = OpenEvent(EVENT_ALL_ACCESS, false, TEXT("CloseEvent"));

	BYTE ACLineStatus = { 0 };
	bool ACStatusChanged = { 0 };

	while (true) {
		DWORD objStat = WaitForMultipleObjects(NUM_HANDLES, EventHandles, FALSE, INFINITE);
		switch (objStat) {
		case WAIT_OBJECT_0:
			ACLineStatus = getPowerStatus();
			ACStatusChanged = CurrentACStatus != ACLineStatus;
			CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
			if (ACStatusChanged) {
				std::cout << "ACLineStatus = " + std::string(CurrentACStatus ? "PLUGGED IN" : "UNPLUGGED") << std::endl;
				SetEvent(EventHandles[0]);
			}
			ResetEvent(EventHandles[0]);
			break;
		case WAIT_OBJECT_0 + 1:
			ResetEvent(EventHandles[1]);
			for (HANDLE& handle : EventHandles)
				CloseHandle(handle);
			return 0;
			break;
		default:
			// Error
			return -1;
		}
	}
	
	for (HANDLE& handle : EventHandles)
		CloseHandle(handle);

	return 0;
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