#include <iostream>
#include <string>
#include <Windows.h>

std::string GetLastErrorAsString();
static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND createHiddenWindow();

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
	DeleteObject(hiddenWindowHandle); //doing it just in case

	return EXIT_SUCCESS;
}

HWND createHiddenWindow() {
	// Create hidden window
	WNDCLASS windowClass = { 0 };
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hInstance = NULL;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.lpszClassName = "Window in Console"; //needs to be the same name
	//when creating the window as well
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	//also register the class
	if (!RegisterClass(&windowClass))
		MessageBox(NULL, "Could not register class", "Error", MB_OK);
	HWND windowHandle = CreateWindow("Window in Console",
		NULL,
		WS_POPUP, //borderless
		0, //x coordinate of window start point
		0, //y start point
		GetSystemMetrics(SM_CXSCREEN), //width of window; this function
		//retrieves the screen resolution.
		GetSystemMetrics(SM_CYSCREEN), //height of the window
		NULL, //handles and such, not needed
		NULL,
		NULL,
		NULL);

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
		std::string ACStatusMessage = ACLineStatus ? "PLUGGED IN" : "UNPLUGGED";
		std::cout << "ACLineStatus = " + ACStatusMessage << std::endl;
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