#pragma once
#include "Settings.h"
#include "tinyformat.h"

enum class LogLevel : unsigned { Off, Fatal, Error, Warn, Info, Debug, Trace };

void sdlog(const char* fmt, tfm::FormatListRef formatList);

#ifdef RELEASE_VER
#define SDLOG(_level, _str, ...)                                                                   \
  {}
#else
template <typename... Args> void SDLOG(LogLevel _level, const char* _str, const Args&... args) {
  if (Settings::get().getLogLevel() >= static_cast<unsigned>(_level) && _level != LogLevel::Off) {
    tfm::FormatListRef formatList = tfm::makeFormatList(args...);
    sdlog(_str, formatList);
  }
}
#endif
