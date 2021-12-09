#include "Settings.h"
#include "WindowManager.h"
#include "defer.h"
#include "util.h"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

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
    std::wstring propertyName;
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
      spdlog::error("performLanguageOverrride: {}", e.what());
    }
  }
  curFPSlimit = getFPSLimit();
}

void Settings::report() {
  spdlog::info("= Settings read:");
#define SETTING(_type, _var, _propertyName, _defaultval)                                           \
  spdlog::info(L" - {} : {}", _propertyName, _var);
#include "Settings.inc"
#undef SETTING
  spdlog::info("_____________");
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
      spdlog::error(L"Language set to {}", (wchar_t*)prevLang);
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
    spdlog::info(L"Set Language key to {}, stored previous value {}", getOverrideLanguage(),
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
      spdlog::info(L"Restored previous language value {}", (wchar_t*)prevLang);
    }
  } catch (const std::system_error& err) {
    spdlog::error("undoLanguageOverride: {}", err.what());
  }
}
