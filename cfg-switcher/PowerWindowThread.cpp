#include <Windows.h>
#include <string>
#include <iostream>
#include "PowerWindowThread.h"
#include "WinUtils.h"

BYTE CurrentACStatus;

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
	if (uMsg == WM_POWERBROADCAST) {
		SYSTEM_POWER_STATUS lpSystemPowerStatus;
		if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
			std::string errMsg = "Error getting system power status: " + GetLastErrorAsString();
			std::cerr << errMsg << std::endl;
			return EXIT_FAILURE;
		}
		BYTE ACLineStatus = lpSystemPowerStatus.ACLineStatus;

		bool ACStatusChanged = CurrentACStatus != ACLineStatus;
		CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
		if (ACStatusChanged) {
			std::cout << "ACLineStatus = " + std::string(CurrentACStatus ? "PLUGGED IN" : "UNPLUGGED") << std::endl;
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
