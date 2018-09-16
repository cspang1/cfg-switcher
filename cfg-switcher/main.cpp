#include <iostream>
#include <string>
#include <Windows.h>

std::string GetLastErrorAsString();

int main() {
	// Register application to receive power setting event notifications
	
	HANDLE hRecipient = GetCurrentProcess();
	LPCGUID PowerSettingGuid = &GUID_ACDC_POWER_SOURCE;
	DWORD Flags = DEVICE_NOTIFY_WINDOW_HANDLE;
	if (RegisterPowerSettingNotification(hRecipient, PowerSettingGuid, Flags) == NULL) {
		std::string errMsg = "Error registering for power setting notifications: " + GetLastErrorAsString();
		std::cerr << errMsg << std::endl;
		return EXIT_FAILURE;
	}
		
	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_QUIT)
		{
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return EXIT_SUCCESS;
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
		std::cout << "ACLineStatus = " << static_cast<int>(ACLineStatus) << std::endl;
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}