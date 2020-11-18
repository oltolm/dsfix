#pragma once
#include <string>
#include <windows.h>

PBYTE GetMemoryAddressFromPattern(const std::string& searchPattern);
void writeToAddress(const void* Data, void* Address, size_t Size);
