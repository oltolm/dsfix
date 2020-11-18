// xmacro file for .ini setting definition
// format: (type, cname, ininame, default value)

// Graphics Options
SETTING(unsigned, FilteringOverride, "filteringOverride", 2);
SETTING(unsigned, RenderWidth, "renderWidth", 1920);
SETTING(unsigned, RenderHeight, "renderHeight", 1080);

SETTING(unsigned, DOFOverrideResolution, "dofOverrideResolution", 540);
SETTING(unsigned, DOFBlurAmount, "dofBlurAmount", 1);

SETTING(unsigned, SsaoStrength, "ssaoStrength", 2);
SETTING(unsigned, AAQuality, "aaQuality", 0);
SETTING(std::string, AAType, "aaType", "FXAA");

SETTING(std::string, SsaoType, "ssaoType", "VSSAO");

SETTING(bool, UnlockFPS, "unlockFPS", true);
SETTING(unsigned, FPSLimit, "FPSlimit", 60);

SETTING(bool, EnableTripleBuffering, "enableTripleBuffering", false);

// Logging Options
SETTING(unsigned, LogLevel, "logLevel", 0);

// Cursor Options
SETTING(bool, DisableCursor, "disableCursor", false);
SETTING(bool, CaptureCursor, "captureCursor", false);

// Language Options
SETTING(std::wstring, OverrideLanguage, "overrideLanguage", L"");

// Skipping Options
SETTING(bool, SkipIntro, "skipIntro", false);

// Wrapper chaining options
SETTING(std::wstring, Dinput8dllWrapper, "dinput8dllWrapper", L"");

// Folder options
SETTING(std::wstring, ScreenshotDir, "screenshotDir", L".");

// Texture Override Options
SETTING(bool, EnableTextureDumping, "enableTextureDumping", false);
SETTING(bool, EnableTextureOverride, "enableTextureOverride", false);

// HUD options
SETTING(bool, EnableHudMod, "enableHudMod", false)
SETTING(bool, EnableMinimalHud, "enableMinimalHud", false)
SETTING(float, HudScaleFactor, "hudScaleFactor", 1.0f)
SETTING(float, HudTopLeftOpacity, "hudTopLeftOpacity", 1.0f)
SETTING(float, HudBottomLeftOpacity, "hudBottomLeftOpacity", 1.0f)
SETTING(float, HudBottomRightOpacity, "hudBottomRightOpacity", 1.0f)

// Screen Options
SETTING(bool, BorderlessFullscreen, "borderlessFullscreen", false);
SETTING(bool, ForceFullscreen, "forceFullscreen", false);
SETTING(bool, ForceWindowed, "forceWindowed", false);
SETTING(unsigned, PresentWidth, "presentWidth", 0);
SETTING(unsigned, PresentHeight, "presentHeight", 0);
SETTING(bool, EnableVsync, "enableVsync", false);
SETTING(unsigned, FullscreenHz, "fullscreenHz", 60);
SETTING(int, D3DAdapterOverride, "d3dAdapterOverride", -1);
