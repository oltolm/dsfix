#include "Settings.h"
#include "WindowManager.h"
#include "util.h"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <Windows.h>

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
  spdlog::info(" - {} : {}", _propertyName, _var);
#include "Settings.inc"
#undef SETTING
  spdlog::info("_____________");
}

void Settings::init() {
  if (!initialized) {
    WindowManager::get().toggleCursorVisibility();
    WindowManager::get().toggleCursorCapture();
    WindowManager::get().toggleBorderlessFullscreen();
    initialized = true;
  }
}

void Settings::shutdown() {
  if (initialized) {
    initialized = false;
  }
}
