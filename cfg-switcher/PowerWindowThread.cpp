#include <Windows.h>
#include <string>
#include <iostream>
#include <atomic>
#include "PowerWindowThread.h"
#include "WinUtils.h"

HANDLE PowerEvent;
HANDLE CloseEvent;

unsigned int __stdcall windowsPowerThread(void* data) {
	HWND hiddenWindowHandle = createHiddenWindow();
	*static_cast<std::atomic<HWND>*>(data) = hiddenWindowHandle;

	// Get power event handle
	PowerEvent = OpenEvent(EVENT_ALL_ACCESS, false, TEXT("PowerEvent"));
	ResetEvent(PowerEvent);
	CloseEvent = OpenEvent(EVENT_ALL_ACCESS, false, TEXT("CloseEvent"));
	ResetEvent(CloseEvent);

	if (!PowerEvent || !CloseEvent) {
		std::cerr << "Error creating event handles: " + GetLastErrorAsString() << std::endl;
		return EXIT_FAILURE;
	}

	// Poll power status
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Destroy hidden window
	DeleteObject(hiddenWindowHandle);

	// Signal API thread to terminate
	SetEvent(CloseEvent);

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

static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_POWERBROADCAST:
			SetEvent(PowerEvent);
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
