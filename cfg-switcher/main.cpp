#include <iostream>
#include <string>
#include <Windows.h>

std::string GetLastErrorAsString();

int main() {
	// Register application to receive power setting event notifications
	
	HANDLE hRecipient;
	LPCGUID PowerSettingGuid;
	DWORD Flags;
	
	RegisterPowerSettingNotification(hRecipient, PowerSettingGuid, Flags);
	
	SYSTEM_POWER_STATUS lpSystemPowerStatus;
	char cont = 'y';
	
	while (cont == 'y') {
		if (!GetSystemPowerStatus(&lpSystemPowerStatus)) {
			std::string errMsg = "Error getting system power status: " + GetLastErrorAsString();
			std::cerr << errMsg << std::endl;
			return EXIT_FAILURE;
		}

		BYTE ACLineStatus = lpSystemPowerStatus.ACLineStatus;
		std::cout << "ACLineStatus = " << static_cast<int>(ACLineStatus) << std::endl;
		std::cout << "Refresh? <y/n>" << std::endl;
		std::cin >> cont;
	}

	return EXIT_SUCCESS;
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