#pragma once
#include <filesystem>
#include <istream>
#include <string>
#include <system_error>
#include <Windows.h>

namespace fs = std::filesystem;

fs::path GetModuleFileNamePath(HMODULE hModule);
fs::path GetSystemDirectoryPath();
fs::path GetModuleDirectoryPath(HMODULE hModule = nullptr);
std::wstring GetLastErrorString();

std::istream& operator>>(std::istream& is, std::wstring& s);

inline HRESULT ThrowIfFailed(HRESULT hr) {
  if (FAILED(hr))
    throw std::system_error(hr, std::system_category());
  return hr;
}

#ifdef _MSC_VER
#define DXGetErrorString9W(hr) std::to_wstring(hr)
#define DXGetErrorDescription9W(hr) std::to_wstring(hr)
#endif
