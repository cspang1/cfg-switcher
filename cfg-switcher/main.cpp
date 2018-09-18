#include <iostream>
#include <string>
#include <atomic>
#include <Windows.h>
#include <process.h>
#include "PowerWindowThread.h"
#include "ConfigSwitchThread.h"
#include "CfgSwitchAPI.h"

const int NUM_THREADS = 2;

int main() {
	std::atomic<HWND> hiddenWindowHandle(NULL);
	HANDLE ThreadHandles[NUM_THREADS];
	ThreadHandles[0] = (HANDLE)_beginthreadex(0, 0, &windowsPowerThread, (void*)&hiddenWindowHandle, 0, 0);
	ThreadHandles[1] = (HANDLE)_beginthreadex(0, 0, &configSwitchThread, 0, 0, 0);
	while(hiddenWindowHandle == NULL);

	if (!initSettings())
		return EXIT_FAILURE;

	std::cout << "Press any key to exit..." << std::endl;
	std::cin.get();

	SendMessage(hiddenWindowHandle, WM_DESTROY, NULL, NULL);

	WaitForMultipleObjects(NUM_THREADS, ThreadHandles, TRUE, INFINITE);

	deleteSettingsFile();

	return EXIT_SUCCESS;
}
