// xmacro file for .ini setting definition
// format: (type, cname, ininame, default value)

// Graphics Options
SETTING(unsigned, FilteringOverride, L"filteringOverride", 2);
SETTING(unsigned, RenderWidth, L"renderWidth", 1920);
SETTING(unsigned, RenderHeight, L"renderHeight", 1080);

SETTING(unsigned, DOFOverrideResolution, L"dofOverrideResolution", 540);
SETTING(unsigned, DOFBlurAmount, L"dofBlurAmount", 1);

SETTING(unsigned, SsaoStrength, L"ssaoStrength", 2);
SETTING(unsigned, AAQuality, L"aaQuality", 0);
SETTING(std::wstring, AAType, L"aaType", L"FXAA");

SETTING(std::wstring, SsaoType, L"ssaoType", L"VSSAO");

SETTING(bool, UnlockFPS, L"unlockFPS", true);
SETTING(unsigned, FPSLimit, L"FPSlimit", 60);

SETTING(bool, EnableTripleBuffering, L"enableTripleBuffering", false);

// Logging Options
SETTING(unsigned, LogLevel, L"logLevel", 0);

// Cursor Options
SETTING(bool, DisableCursor, L"disableCursor", false);
SETTING(bool, CaptureCursor, L"captureCursor", false);

// Language Options
SETTING(std::wstring, OverrideLanguage, L"overrideLanguage", L"");

// Wrapper chaining options
SETTING(std::wstring, Dinput8dllWrapper, L"dinput8dllWrapper", L"");

// Folder options
SETTING(std::wstring, ScreenshotDir, L"screenshotDir", L".");

// HUD options
SETTING(bool, EnableHudMod, L"enableHudMod", false)
SETTING(bool, EnableMinimalHud, L"enableMinimalHud", false)
SETTING(float, HudScaleFactor, L"hudScaleFactor", 1.0f)
SETTING(float, HudTopLeftOpacity, L"hudTopLeftOpacity", 1.0f)
SETTING(float, HudBottomLeftOpacity, L"hudBottomLeftOpacity", 1.0f)
SETTING(float, HudBottomRightOpacity, L"hudBottomRightOpacity", 1.0f)

// Screen Options
SETTING(bool, BorderlessFullscreen, L"borderlessFullscreen", false);
SETTING(bool, ForceFullscreen, L"forceFullscreen", false);
SETTING(bool, ForceWindowed, L"forceWindowed", false);
SETTING(unsigned, PresentWidth, L"presentWidth", 0);
SETTING(unsigned, PresentHeight, L"presentHeight", 0);
SETTING(bool, EnableVsync, L"enableVsync", false);
SETTING(unsigned, FullscreenHz, L"fullscreenHz", 60);
SETTING(int, D3DAdapterOverride, L"d3dAdapterOverride", -1);
