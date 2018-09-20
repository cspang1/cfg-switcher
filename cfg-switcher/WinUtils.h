#pragma once
#include <string>

std::string GetLastErrorAsString();
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
std::string BrowseFolder(std::string prompt);
std::string BrowseFile(std::string prompt);
std::string FileFromPath(std::string filePath);
bool copyFile(const char *SRC, const char* DEST);
