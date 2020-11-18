#include "WindowManager.h"

WindowManager WindowManager::instance;

void WindowManager::applyCursorCapture() {
  if (captureCursor) {
    RECT clientrect;
    HWND hwnd = ::GetActiveWindow();
    ::GetClientRect(hwnd, &clientrect);
    ::ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&clientrect.left));
    ::ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&clientrect.right));
    ::ClipCursor(&clientrect);
  } else {
    ::ClipCursor(nullptr);
  }
}

void WindowManager::toggleCursorCapture() { captureCursor = !captureCursor; }

void WindowManager::toggleCursorVisibility() {
  cursorVisible = !cursorVisible;
  ::ShowCursor(cursorVisible);
}

void WindowManager::toggleBorderlessFullscreen(bool enable) {
  HWND hwnd = ::GetActiveWindow();
  if (enable) {
    // set styles
    LONG lStyle = prevStyle = ::GetWindowLongW(hwnd, GWL_STYLE);
    lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    ::SetWindowLongW(hwnd, GWL_STYLE, lStyle);
    LONG lExStyle = prevExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
    lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    ::SetWindowLongW(hwnd, GWL_EXSTYLE, lExStyle);
    // adjust size & position
    HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {};
    info.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfo(monitor, &info);
    int monitorWidth = info.rcMonitor.right - info.rcMonitor.left;
    int monitorHeight = info.rcMonitor.bottom - info.rcMonitor.top;
    ::SetWindowPos(hwnd, nullptr, info.rcMonitor.left, info.rcMonitor.top, monitorWidth,
                   monitorHeight, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
  } else if (prevStyle != 0) {
    // restore previous window
    ::SetWindowLongW(hwnd, GWL_STYLE, prevStyle);
    ::SetWindowLongW(hwnd, GWL_EXSTYLE, prevExStyle);
  }
}
