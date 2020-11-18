#pragma once
#include <Windows.h>

class WindowManager {
  static WindowManager instance;
  bool captureCursor = false, cursorVisible = true;
  long prevStyle = 0, prevExStyle = 0;

public:
  static WindowManager& get() { return instance; }
  WindowManager() {}
  void applyCursorCapture();
  void toggleCursorCapture();
  void toggleCursorVisibility();
  void toggleBorderlessFullscreen(bool enable);
};
