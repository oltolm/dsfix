#pragma once
#include <Windows.h>

class WindowManager {
  static WindowManager instance;
  bool captureCursor = false, cursorVisible = true;
  bool borderlessFullscreen = false;
  RECT prevWindowRect;
  long prevStyle = 0, prevExStyle = 0;

public:
  static WindowManager& get() { return instance; }
  WindowManager() {}
  void applyCursorCapture();
  void toggleCursorCapture();
  void toggleCursorVisibility();
  void toggleBorderlessFullscreen();
  void resize(unsigned clientW, unsigned clientH);
};
