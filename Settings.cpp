#include "Settings.h"
#include "WindowManager.h"
#include "util.h"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <Windows.h>

std::ostream& operator<<(std::ostream& os, const std::wstring& s);

std::ostream& operator<<(std::ostream& os, const wchar_t* s);

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
  curFPSlimit = getFPSLimit();
}

void Settings::save() {
  fs::path iniFilename = GetModuleDirectoryPath() / L"DSfix.ini";
  std::ofstream settings(iniFilename);

#define SETTING(_type, _var, _propertyName, _defaultval)                                           \
  settings << _propertyName << " " << _var << "\n";
#include "Settings.inc"
#undef SETTING
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
      WindowManager::get().toggleBorderlessFullscreen(true);
    initialized = true;
  }
}

void Settings::shutdown() {
  if (initialized) {
    initialized = false;
  }
}

unsigned int Settings::getCurrentFPSLimit() { return curFPSlimit; }

void Settings::setCurrentFPSLimit(unsigned limit) { curFPSlimit = limit; }

void Settings::toggle30FPSLimit() { curFPSlimit = (curFPSlimit == 30) ? getFPSLimit() : 30; }
