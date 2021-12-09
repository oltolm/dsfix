#include "KeyActions.h"
#include "FPS.h"
#include "RenderstateManager.h"
#include "Settings.h"
#include "WindowManager.h"
#include "util.h"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

KeyActions KeyActions::instance;

void KeyActions::load() {
  fs::path iniFilename = GetModuleDirectoryPath() / L"DSfixKeys.ini";
  std::ifstream settings(iniFilename);
  std::string line;
  while (std::getline(settings, line)) {
    if (line[0] == '#' || line.empty())
      continue;
    std::string action, keyName;
    std::istringstream iss(line);
    iss >> action;
    iss >> keyName;
#define KEY(_name, _val)                                                                           \
  if (keyName == #_name) {                                                                         \
    keyBindingMap.insert({_val, action});                                                          \
    continue;                                                                                      \
  }
#include "Keys.inc"
#undef KEY
  }
}

void KeyActions::report() {
  spdlog::info("= Loaded Keybindings:");
  for (const auto& keyActionPair : keyBindingMap) {
    spdlog::info(" - {:p} => {}", keyActionPair.first, keyActionPair.second);
  }
  spdlog::info("=============");
}

void KeyActions::performAction(const std::string& name) {
#define ACTION(_name, _action)                                                                     \
  if (name.compare(#_name) == 0)                                                                   \
    _name();
#include "Actions.inc"
#undef ACTION
}

void KeyActions::processIO() {
  if (::GetForegroundWindow() != nullptr && ::GetActiveWindow() != nullptr) {
    for (auto& keyActionPair : keyBindingMap) {
      if (::GetAsyncKeyState(keyActionPair.first) & 1) {
        performAction(keyActionPair.second);
      }
    }
  }
}

#define ACTION(_name, _action)                                                                     \
  void KeyActions::_name() { _action; };
#include "Actions.inc"
#undef ACTION
