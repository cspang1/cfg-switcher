#include <iostream>
#include <string>
#include <Windows.h>
#include "PowerWindowThread.h"

int main() {
	HWND hiddenWindowHandle = createHiddenWindow();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_QUIT)
		{
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(hiddenWindowHandle);

	return EXIT_SUCCESS;
}
