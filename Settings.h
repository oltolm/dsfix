#pragma once
#include <Windows.h>
#include <string>

class Settings {
  static Settings instance;
  bool initialized = false, langOverridden = false;
  unsigned curFPSlimit = 0;
#define SETTING(_type, _var, _inistring, _defaultval)                                              \
private:                                                                                           \
  _type _var = _defaultval;                                                                        \
                                                                                                   \
public:                                                                                            \
  _type get##_var() const { return _var; };
#include "Settings.inc"
#undef SETTING
public:
  static Settings& get() { return instance; }
  void load();
  void performLanguageOverride();
  void report();
  void init();
  void shutdown();
  void undoLanguageOverride();
  Settings() = default;
  unsigned getCurrentFPSLimit();
  void setCurrentFPSLimit(unsigned limit);
  void toggle30FPSLimit();
};
