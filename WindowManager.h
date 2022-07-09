#pragma once
#include <Windows.h>

class WindowManager {
  static WindowManager instance;

public:
  static WindowManager& get() { return instance; }

  WindowManager() {}

  void toggleCursorCapture();
  void toggleCursorVisibility();
  void toggleBorderlessFullscreen();
};
