#pragma once
#include <string>
#include <Windows.h>

std::string GetLastErrorAsString();
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
std::string BrowseFolder(std::string prompt);
std::string BrowseFile(std::string prompt);
std::string FileFromPath(std::string filePath);
bool copyFile(const char *SRC, const char* DEST);
int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories = true);
std::string GetCurrentWorkingDir();
