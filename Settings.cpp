#include "Settings.h"
#include "WindowManager.h"
#include "defer.h"
#include "log.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

Settings Settings::instance;

void Settings::load() {
  fs::path iniFilename = GetModuleDirectoryPath() / L"DSfix.ini";
  std::ifstream settings(iniFilename);
  std::string line;
  while (std::getline(settings, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream iss(line);
    std::string propertyName;
    iss >> propertyName;
#define SETTING(_type, _var, _propertyName, _defaultval)                                           \
  if (propertyName == _propertyName) {                                                             \
    iss >> _var;                                                                                   \
    continue;                                                                                      \
  }
#include "Settings.inc"
#undef SETTING
  }
  if (getPresentWidth() == 0)
    PresentWidth = getRenderWidth();
  if (getPresentHeight() == 0)
    PresentHeight = getRenderHeight();
  if (getOverrideLanguage().length() >= 2 && getOverrideLanguage().find(L"none") != 0) {
    try {
      performLanguageOverride();
    } catch (const std::system_error& e) {
      SDLOG(LogLevel::Error, "performLanguageOverrride: %s", e.what());
    }
  }
  curFPSlimit = getFPSLimit();
}

void Settings::report() {
  SDLOG(LogLevel::Info, "= Settings read:");
#define SETTING(_type, _var, _propertyName, _defaultval)                                           \
  SDLOG(LogLevel::Info, " - %s : %s", _propertyName, _var);
#include "Settings.inc"
#undef SETTING
  SDLOG(LogLevel::Info, "_____________");
}

void Settings::init() {
  if (!initialized) {
    if (getDisableCursor())
      WindowManager::get().toggleCursorVisibility();
    if (getCaptureCursor())
      WindowManager::get().toggleCursorCapture();
    if (getBorderlessFullscreen())
      WindowManager::get().toggleBorderlessFullscreen();
    WindowManager::get().resize(0, 0);
    initialized = true;
  }
}

void Settings::shutdown() {
  if (initialized) {
    undoLanguageOverride();
    initialized = false;
  }
}

unsigned Settings::getCurrentFPSLimit() { return curFPSlimit; }

void Settings::setCurrentFPSLimit(unsigned limit) { curFPSlimit = limit; }

void Settings::toggle30FPSLimit() { curFPSlimit = (curFPSlimit == 30) ? getFPSLimit() : 30; }

// language override
void Settings::performLanguageOverride() {
  LSTATUS ret;
  BYTE prevLang[16]; // previous locale registry key
  DWORD prevLangSize;
  {
    HKEY key = nullptr;
    DEFER(if (key != nullptr)::RegCloseKey(key););
    // Reading operations
    ret = throw_if_not_error_success(
        ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\International", 0, KEY_READ, &key));
    // check if prev key already set -- if so, assume correct override and return
    ret = ::RegQueryValueExW(key, L"PrevLocaleName", 0, 0, prevLang, &prevLangSize);
    if (ret == ERROR_SUCCESS)
      return;
    // read current locale
    ret = throw_if_not_error_success(
        ::RegQueryValueExW(key, L"LocaleName", 0, 0, prevLang, &prevLangSize));
    // if locale already set: no override necessary
    if (getOverrideLanguage().find((wchar_t*)prevLang) == 0) {
      SDLOG(LogLevel::Error, "Language set to %s", (wchar_t*)prevLang);
      return;
    }
  }
  {
    HKEY key = nullptr;
    DEFER(if (key != nullptr) {
      ::RegFlushKey(key);
      ::RegCloseKey(key);
    });
    // Writing operations
    ret = throw_if_not_error_success(
        ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\International", 0, KEY_WRITE, &key));
    // store previous locale
    ret = throw_if_not_error_success(
        ::RegSetValueExW(key, L"PrevLocaleName", 0, REG_SZ, prevLang, prevLangSize));
    // override existing locale
    ret = throw_if_not_error_success(
        ::RegSetValueExW(key, L"LocaleName", 0, REG_SZ, (BYTE*)getOverrideLanguage().c_str(),
                         (getOverrideLanguage().length() + 1) * sizeof(wchar_t)));
    SDLOG(LogLevel::Info, "Set Language key to %s, stored previous value %s", getOverrideLanguage(),
          (wchar_t*)prevLang);
  }
}

void Settings::undoLanguageOverride() {
  try {
    LSTATUS ret;
    BYTE prevLang[32]; // previous locale registry key
    DWORD prevLangSize;
    {
      HKEY key = nullptr;
      DEFER(if (key != nullptr)::RegCloseKey(key););
      // reading operations
      ret = throw_if_not_error_success(
          ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\International", 0, KEY_READ, &key));
      // load previous locale
      ret = ::RegQueryValueExW(key, L"PrevLocaleName", 0, 0, prevLang, &prevLangSize);
      if (ret != ERROR_SUCCESS)
        return;
    }
    {
      HKEY key = nullptr;
      DEFER(if (key != nullptr) {
        ::RegFlushKey(key);
        ::RegCloseKey(key);
      });
      // Writing operations
      ret = throw_if_not_error_success(
          ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\International", 0, KEY_WRITE, &key));
      // restore previous locale
      ret = throw_if_not_error_success(
          ::RegSetValueExW(key, L"LocaleName", 0, REG_SZ, prevLang, prevLangSize));
      // remove PrevLocaleName value
      ret = throw_if_not_error_success(::RegDeleteValueW(key, L"PrevLocaleName"));
      SDLOG(LogLevel::Info, "Restored previous language value %s", (wchar_t*)prevLang);
    }
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "undoLanguageOverride: %s", err.what());
  }
}
