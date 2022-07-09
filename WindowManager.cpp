#include "WindowManager.h"
#include "Settings.h"

WindowManager WindowManager::instance;

void WindowManager::toggleCursorCapture() {
  if (Settings::get().getCaptureCursor()) {
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

void WindowManager::toggleCursorVisibility() { ::ShowCursor(!Settings::get().getDisableCursor()); }

void WindowManager::toggleBorderlessFullscreen() {
  HWND hwnd = ::GetActiveWindow();
  if (Settings::get().getBorderlessFullscreen()) {
    // set styles
    LONG lStyle = ::GetWindowLongW(hwnd, GWL_STYLE);
    lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    ::SetWindowLongW(hwnd, GWL_STYLE, lStyle);
    LONG lExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
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
  } else {
    // restore previous window
    LONG lStyle = ::GetWindowLongW(hwnd, GWL_STYLE);
    LONG lExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
    lStyle |= (WS_DLGFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU);
    lExStyle |= WS_EX_WINDOWEDGE;
    ::SetWindowLongW(hwnd, GWL_STYLE, lStyle);
    ::SetWindowLongW(hwnd, GWL_EXSTYLE, lExStyle);
  }
}
