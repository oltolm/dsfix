#pragma once
#include <string>

class Settings {
  static Settings instance;
  bool initialized = false;
#define SETTING(_type, _var, _inistring, _defaultval)                                              \
private:                                                                                           \
  _type _var = _defaultval;                                                                        \
                                                                                                   \
public:                                                                                            \
  _type get##_var() const { return _var; };                                                        \
  void set##_var(_type _var) { this->_var = _var; };
#include "Settings.inc"
#undef SETTING
public:
  static Settings& get() { return instance; }

  void load();
  void save();
  void report();
  void init();
  void shutdown();
  Settings() = default;
};
