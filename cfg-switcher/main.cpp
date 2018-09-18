#include <iostream>
#include <string>
#include <Windows.h>
#include <process.h>
#include <future>
#include "WinUtils.h"
#include "PowerWindowThread.h"
#include "ConfigSwitchThread.h"
#include "CfgSwitchAPI.h"

const int NUM_THREADS = 2;
const int TIMEOUT = 5000;

int main() {
	HANDLE PowerEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("PowerEvent"));
	HANDLE CloseEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("CloseEvent"));
	if (!PowerEvent || !CloseEvent) {
		std::cerr << "Error creating event handles: " + GetLastErrorAsString() << std::endl;
		return EXIT_FAILURE;
	}

	std::promise<HWND> pwhPromise;
	std::future<HWND> pwhFuture = pwhPromise.get_future();

	HANDLE ThreadHandles[NUM_THREADS];
	ThreadHandles[0] = (HANDLE)_beginthreadex(0, 0, &windowsPowerThread, (void *)&pwhPromise, 0, 0);
	ThreadHandles[1] = (HANDLE)_beginthreadex(0, 0, &configSwitchThread, 0, 0, 0);
	for (HANDLE &th : ThreadHandles) {
		if (!th) {
			CloseHandle(PowerEvent);
			std::cerr << "Error creating threads: " + GetLastErrorAsString() << std::endl;
			return EXIT_FAILURE;
		}
	}

	if (pwhFuture.wait_for(std::chrono::milliseconds(TIMEOUT)) != std::future_status::ready) {
		std::cerr << "Error getting handle to hidden power window..." << std::endl;
		return EXIT_FAILURE;
	}

	if (!initSettings())
		return EXIT_FAILURE;

	std::cout << "Press any key to exit..." << std::endl;
	std::cin.get();

	SendMessage(pwhFuture.get(), WM_DESTROY, NULL, NULL);

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
