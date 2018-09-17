#include <Windows.h>
#include <string>
#include <iostream>
#include <atomic>
#include "PowerWindowThread.h"
#include "WinUtils.h"

BYTE CurrentACStatus;

unsigned int __stdcall windowsPowerThread(void* data)
{
	HWND hiddenWindowHandle = createHiddenWindow();
	*static_cast<std::atomic<HWND>*>(data) = hiddenWindowHandle;

	std::cout << "THREAD: " << hiddenWindowHandle << std::endl;

	// Perform initial power status check
	CurrentACStatus = getPowerStatus();

	// Poll power status
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		std::cout << "MESSAGES!!!!" << std::endl;

		if (msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Destroy hidden window
	DeleteObject(hiddenWindowHandle);

	return 0;
}

HWND createHiddenWindow() {
	// Create hidden window
	WNDCLASS windowClass = { 0 };
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hInstance = NULL;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.lpszClassName = "Window in Console";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClass(&windowClass))
		MessageBox(NULL, "Could not register class", "Error", MB_OK);
	HWND windowHandle = CreateWindow("Window in Console",
		NULL,
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		NULL,
		NULL
	);

	return windowHandle;
}

static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BYTE ACLineStatus = { 0 };
	bool ACStatusChanged = { 0 };
	switch (uMsg) {
		case WM_POWERBROADCAST:
			ACLineStatus = getPowerStatus();
			ACStatusChanged = CurrentACStatus != ACLineStatus;
			CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
			if (ACStatusChanged) {
				std::cout << "ACLineStatus = " + std::string(CurrentACStatus ? "PLUGGED IN" : "UNPLUGGED") << std::endl;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BYTE getPowerStatus()
{
	SYSTEM_POWER_STATUS lpSystemPowerStatus;
	if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
		std::string errMsg = "Error getting system power status: " + GetLastErrorAsString();
		std::cerr << errMsg << std::endl;
		return EXIT_FAILURE;
	}

	return lpSystemPowerStatus.ACLineStatus;
}
