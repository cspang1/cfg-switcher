#pragma once
#include <Windows.h>

unsigned int __stdcall windowsPowerThread(void* data);
HWND createHiddenWindow();
static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BYTE getPowerStatus();