#pragma once

#include <Windows.h>

HWND createHiddenWindow();
static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);