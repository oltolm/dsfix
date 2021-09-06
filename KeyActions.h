#pragma once
#include <map>
#include <string>
class KeyActions {
  static KeyActions instance;
  using IntStrMap = std::map<int, std::string>;
  IntStrMap keyBindingMap;
#define ACTION(_name, _action) void _name();
#include "Actions.inc"
#undef ACTION
  void performAction(const std::string& name);

public:
  static KeyActions& get() { return instance; }
  KeyActions() = default;
  void load();
  void report();
  void processIO();
};
