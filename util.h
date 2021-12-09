#pragma once
#include <Windows.h>
#include <filesystem>
#include <spdlog/fmt/ostr.h>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

fs::path GetModuleFileNamePath(HMODULE hModule);
fs::path GetSystemDirectoryPath();
fs::path GetModuleDirectoryPath(HMODULE hModule = nullptr);
std::wstring GetLastErrorString();

std::istream& operator>>(std::istream& is, std::wstring& s);
std::ostream& operator<<(std::ostream& os, const fs::path& p);

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

template <class T> inline T* throw_if_null(T* ptr) {
  if (ptr == nullptr) {
    throw std::system_error(::GetLastError(), std::system_category());
  }
  return ptr;
}

#ifdef _MSC_VER
#define DXGetErrorString9W(hr) std::to_wstring(hr)
#define DXGetErrorDescription9W(hr) std::to_wstring(hr)
#endif
