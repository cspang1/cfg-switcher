#include <iostream>
#include <string>
#include <atomic>
#include <Windows.h>
#include <process.h>
#include "PowerWindowThread.h"

int main() {
	std::atomic<HWND> hiddenWindowHandle(NULL); 
	HANDLE PowerWindowThreadHandle = (HANDLE)_beginthreadex(0, 0, &windowsPowerThread, (void*)&hiddenWindowHandle, 0, 0);

	while(hiddenWindowHandle == NULL);

	char exit = { 0 };
	std::cout << "Type q to exit..." << std::endl;
	while (exit != 'q') 
		std::cin >> exit;

	std::cout << "MAIN: " << hiddenWindowHandle << std::endl;

	//SendMessage(hiddenWindowHandle, WM_DESTROY, NULL, NULL);

	WaitForSingleObject(PowerWindowThreadHandle, INFINITE);
	CloseHandle(PowerWindowThreadHandle);

	return EXIT_SUCCESS;
}
