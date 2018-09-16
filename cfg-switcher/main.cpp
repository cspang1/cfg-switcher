#include <iostream>
#include <string>
#include <Windows.h>
#include <process.h>
#include "PowerWindowThread.h"

int main() {
	HANDLE PowerWindowThreadHandle = (HANDLE)_beginthreadex(0, 0, &windowsPowerThread, 0, 0, 0);

	WaitForSingleObject(PowerWindowThreadHandle, INFINITE);
	CloseHandle(PowerWindowThreadHandle);

	return EXIT_SUCCESS;
}
