#pragma once
#include <string>
#include <windows.h>

DWORD GetMemoryAddressFromPattern(PCWSTR szDllName, const std::string& searchPattern, DWORD offset);
void writeToAddress(const void* Data, void* Address, size_t Size);
