#include <iostream>
#include <string>
#include <Windows.h>
#include <process.h>
#include <future>
#include "WinUtils.h"
#include "PowerWindowThread.h"
#include "ConfigSwitchThread.h"
#include "CfgSwitchAPI.h"

const int NUM_THREAD_HANDLES = 2;
const int NUM_EVENT_HANDLES = 2;
const int NUM_HANDLES = NUM_THREAD_HANDLES + NUM_EVENT_HANDLES;
const int TIMEOUT = 5000;

int main() {
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
	HANDLE SwitchThread = (HANDLE)_beginthreadex(0, 0, &configSwitchThread, 0, 0, 0);
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

	// Attempt to initialize settings
	if (!initSettings())
		return EXIT_FAILURE;

	// Aggregate process handles
	HANDLE ProcHandles[NUM_HANDLES] = {PowerEvent, CloseEvent, WindowThread, SwitchThread};

	std::cout << "Press any key to exit..." << std::endl;
	std::cin.get();

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

	// DEBUG: Delete settings file
	deleteSettingsFile();

	return EXIT_SUCCESS;
}
