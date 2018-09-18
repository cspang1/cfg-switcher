#pragma once
#include <Windows.h>

unsigned int __stdcall windowsPowerThread(void*);
HWND createHiddenWindow();
static LRESULT WINAPI WindowProc(HWND, UINT, WPARAM, LPARAM);