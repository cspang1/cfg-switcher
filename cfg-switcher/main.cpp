#include <iostream>
#include <string>
#include <atomic>
#include <Windows.h>
#include <process.h>
#include "WinUtils.h"
#include "PowerWindowThread.h"
#include "ConfigSwitchThread.h"
#include "CfgSwitchAPI.h"

const int NUM_THREADS = 2;

int main() {
	HANDLE PowerEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("PowerEvent"));
	HANDLE CloseEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("CloseEvent"));
	if (!PowerEvent) {
		std::cerr << "Error creating event handles: " + GetLastErrorAsString() << std::endl;
		return EXIT_FAILURE;
	}

	std::atomic<HWND> hiddenWindowHandle(NULL);
	HANDLE ThreadHandles[NUM_THREADS];
	ThreadHandles[0] = (HANDLE)_beginthreadex(0, 0, &windowsPowerThread, (void*)&hiddenWindowHandle, 0, 0);
	ThreadHandles[1] = (HANDLE)_beginthreadex(0, 0, &configSwitchThread, 0, 0, 0);
	for (HANDLE &th : ThreadHandles) {
		if (!th) {
			CloseHandle(PowerEvent);
			std::cerr << "Error creating threads: " + GetLastErrorAsString() << std::endl;
			return EXIT_FAILURE;
		}
	}

	while(hiddenWindowHandle == NULL);

	if (!initSettings())
		return EXIT_FAILURE;

	std::cout << "Press any key to exit..." << std::endl;
	std::cin.get();

	SendMessage(hiddenWindowHandle, WM_DESTROY, NULL, NULL);

	DWORD threadStatus = WaitForMultipleObjects(NUM_THREADS, ThreadHandles, TRUE, INFINITE);
	switch (threadStatus) {
		case WAIT_OBJECT_0:
			printf("All threads ended, cleaning up for application exit...\n");
			break;

		default:
			printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
			return 1;
	}

	deleteSettingsFile();

	return EXIT_SUCCESS;
}
