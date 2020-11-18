#include "util.h"
#include <stdexcept>
#include <system_error>

std::string toUtf8(const std::wstring& wideCharStr);
std::wstring toUnicode(const std::string& multiByteStr);

std::istream& operator>>(std::istream& is, std::wstring& s) {
  std::string utf8;
  is >> utf8;
  s = toUnicode(utf8);
  return is;
}

std::ostream& operator<<(std::ostream& os, const std::wstring& s) {
  os << toUtf8(s);
  return os;
}

std::ostream& operator<<(std::ostream& os, const wchar_t* s) {
  os << toUtf8(s);
  return os;
}

std::wostream& operator<<(std::wostream& wos, const std::string& s) {
  wos << toUnicode(s);
  return wos;
}

std::wstring FormatMessageString(DWORD dwMessageId) {
  LPWSTR pBuffer = nullptr;
  DWORD length = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                      FORMAT_MESSAGE_IGNORE_INSERTS,
                                  nullptr, dwMessageId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                  (LPWSTR)&pBuffer, 0, nullptr);
  if (length == 0)
    return L"";
  std::wstring result(pBuffer, length);
  LocalFree(pBuffer);
  return result;
}

std::wstring GetLastErrorString() { return FormatMessageString(GetLastError()); }

fs::path GetModuleDirectoryPath(HMODULE hModule) {
  fs::path dllPath = GetModuleFileNamePath(hModule);
  return dllPath.parent_path();
}

fs::path GetModuleFileNamePath(HMODULE hModule) {
  std::wstring filename;
  filename.resize(MAX_PATH);
  bool truncated;
  do {
    DWORD nRet = ::GetModuleFileNameW(hModule, &filename[0], filename.size());
    throw_if_zero(nRet);
    truncated = nRet == filename.size();
    filename.resize(truncated ? filename.size() * 2 : nRet);
  } while (truncated);
  return filename;
}

fs::path GetSystemDirectoryPath() {
  std::wstring buffer;
  buffer.resize(MAX_PATH);
  bool notEnoughSpace;
  do {
    UINT uRet = ::GetSystemDirectoryW(&buffer[0], buffer.size());
    throw_if_zero(uRet);
    notEnoughSpace = uRet > buffer.size();
    buffer.resize(uRet);
  } while (notEnoughSpace);
  return buffer;
}

std::string toUtf8(const std::wstring& wideCharStr) {
  if (wideCharStr.empty())
    return "";
  int cbMultiByte = ::WideCharToMultiByte(CP_UTF8, 0, wideCharStr.data(), wideCharStr.size(),
                                          nullptr, 0, nullptr, nullptr);
  throw_if_zero(cbMultiByte);
  std::string multiByteStr;
  multiByteStr.resize(cbMultiByte);
  cbMultiByte = ::WideCharToMultiByte(CP_UTF8, 0, wideCharStr.data(), wideCharStr.size(),
                                      &multiByteStr[0], cbMultiByte, nullptr, nullptr);
  throw_if_zero(cbMultiByte);
  return multiByteStr;
}

std::wstring toUnicode(const std::string& multiByteStr) {
  if (multiByteStr.empty())
    return L"";
  int cchWideChar =
      ::MultiByteToWideChar(CP_UTF8, 0, multiByteStr.data(), multiByteStr.size(), nullptr, 0);
  throw_if_zero(cchWideChar);
  std::wstring wideCharStr;
  wideCharStr.resize(cchWideChar);
  cchWideChar = ::MultiByteToWideChar(CP_UTF8, 0, multiByteStr.data(), multiByteStr.size(),
                                      &wideCharStr[0], cchWideChar);
  throw_if_zero(cchWideChar);
  return wideCharStr;
}
