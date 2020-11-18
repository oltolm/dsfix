
// Cursor Actions

ACTION(toggleCursorCapture, WindowManager::get().toggleCursorCapture())
ACTION(toggleCursorVisibility, WindowManager::get().toggleCursorVisibility())

// Window Actions

ACTION(toggleBorderlessFullscreen, WindowManager::get().toggleBorderlessFullscreen())

// Screenshot Actions

ACTION(takeHudlessScreenshot, RSManager::get().takeHudlessScreenshot())

// Graphics Actions

ACTION(toggleAA, RSManager::get().toggleAA())
ACTION(toggleDofGauss, RSManager::get().toggleDofGauss())

ACTION(toggleHUD, RSManager::get().toggleHUD())
ACTION(toggleHUDChanges, RSManager::get().toggleHUDChanges())

ACTION(toggle30FPSLimit, Settings::get().toggle30FPSLimit())

// Development Actions

ACTION(userTrigger, SDLOG(LogLevel::Info, "================================================================= USER TRIGGER ==="))

ACTION(togglePaused, RSManager::get().togglePaused());
