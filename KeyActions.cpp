#include "KeyActions.h"
#include "FPS.h"
#include "RenderstateManager.h"
#include "WindowManager.h"
#include "log.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

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
  SDLOG(LogLevel::Info, "= Loaded Keybindings:");
  for (auto& keyActionPair : keyBindingMap) {
    SDLOG(LogLevel::Info, " - %p => %s", keyActionPair.first, keyActionPair.second);
  }
  SDLOG(LogLevel::Info, "=============");
}

void KeyActions::performAction(const std::string& name) {
#define ACTION(_name, _action)                                                                     \
  if (name.compare(#_name) == 0)                                                                   \
    _name();
#include "Actions.inc"
#undef ACTION
}

void KeyActions::processIO() {
  if (::GetForegroundWindow() != NULL && ::GetActiveWindow() != NULL) {
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
