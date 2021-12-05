#pragma once

#include <windows.h>

DWORD GetMemoryAddressFromPattern(LPWSTR szDllName, LPCSTR szSearchPattern, DWORD offset);
