#pragma once

#include <windows.h>

DWORD GetMemoryAddressFromPattern(LPCWSTR szDllName, LPCSTR szSearchPattern, DWORD offset);
