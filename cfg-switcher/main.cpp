#include <iostream>
#include <string>
#include <atomic>
#include <Windows.h>
#include <process.h>
#include "PowerWindowThread.h"
#include "CfgSwitchAPI.h"

int main() {
	std::atomic<HWND> hiddenWindowHandle(NULL); 
	HANDLE PowerWindowThreadHandle = (HANDLE)_beginthreadex(0, 0, &windowsPowerThread, (void*)&hiddenWindowHandle, 0, 0);
	while(hiddenWindowHandle == NULL);

	if (!initSettings()) std::cout << "No settings file yet..." << std::endl;
	else std::cout << "Settings file found!" << std::endl;

	std::cout << "Press any key to exit..." << std::endl;
	std::cin.get();

	SendMessage(hiddenWindowHandle, WM_DESTROY, NULL, NULL);

	WaitForSingleObject(PowerWindowThreadHandle, INFINITE);
	CloseHandle(PowerWindowThreadHandle);

	return EXIT_SUCCESS;
}
