#pragma once
#include "log.h"
#include "tinyformat.h"
#include <Windows.h>
#include <dxerr9.h>
#include <string>
#include <system_error>
#include <filesystem>

namespace fs = std::filesystem;

fs::path GetModuleFileNamePath(HMODULE hModule);
fs::path GetModuleDirectory(HMODULE hModule);
fs::path GetSystemDirectoryPath();
fs::path GetModuleDirectoryPath(HMODULE hModule = NULL);
std::wstring GetLastErrorString();
std::wstring FormatMessageString(DWORD dwMessageId);
std::ostream& operator<<(std::ostream& os, const std::wstring& s);
std::ostream& operator<<(std::ostream& os, const wchar_t* s);
std::istream& operator>>(std::istream& is, std::wstring& s);
std::wostream& operator<<(std::wostream& wos, const std::string& s);

inline HRESULT throw_if_fail(HRESULT hr) {
  if (FAILED(hr)) {
    throw std::system_error(hr, std::system_category());
  }
  return hr;
}

inline LSTATUS throw_if_not_error_success(LSTATUS status) {
  if (status != ERROR_SUCCESS) {
    throw std::system_error(status, std::system_category());
  }
  return status;
}

inline DWORD throw_if_zero(DWORD status) {
  if (status == 0) {
    throw std::system_error(::GetLastError(), std::system_category());
  }
  return status;
}

template<class T>
inline T throw_if_null(T ptr) {
  if (ptr == nullptr) {
    throw std::system_error(::GetLastError(), std::system_category());
  }
  return ptr;
}
