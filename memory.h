#pragma once

#include <string>
#include <windows.h>

DWORD GetMemoryAddressFromPattern(PCWSTR szDllName, const std::string& searchPattern, DWORD offset);
