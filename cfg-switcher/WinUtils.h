#pragma once
#include <string>

std::string GetLastErrorAsString();
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
std::string BrowseFolder(std::string saved_path);
