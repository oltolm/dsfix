// xmacro file for .ini setting definition
// format: (type, cname, ininame, default value)

// Graphics Options
SETTING(unsigned int, FilteringOverride, "filteringOverride", 2);
SETTING(unsigned int, RenderWidth, "renderWidth", 1920);
SETTING(unsigned int, RenderHeight, "renderHeight", 1080);

SETTING(unsigned int, DOFOverrideResolution, "dofOverrideResolution", 1079);
SETTING(unsigned int, DOFBlurAmount, "dofBlurAmount", 1);

SETTING(unsigned int, SsaoStrength, "ssaoStrength", 3);
SETTING(unsigned int, AAQuality, "aaQuality", 4);
SETTING(std::string, AAType, "aaType", "FXAA");

SETTING(std::string, SsaoType, "ssaoType", "VSSAO2");

SETTING(bool, UnlockFPS, "unlockFPS", true);
SETTING(unsigned int, FPSLimit, "FPSlimit", 60);

// Logging Options
SETTING(unsigned int, LogLevel, "logLevel", 0);

// Cursor Options
SETTING(bool, DisableCursor, "disableCursor", false);
SETTING(bool, CaptureCursor, "captureCursor", false);

// Skipping Options
SETTING(bool, SkipIntro, "skipIntro", false);

// Screen Options
SETTING(bool, BorderlessFullscreen, "borderlessFullscreen", false);
SETTING(unsigned int, PresentWidth, "presentWidth", 0);
SETTING(unsigned int, PresentHeight, "presentHeight", 0);
SETTING(bool, EnableVsync, "enableVsync", false);
SETTING(unsigned int, FullscreenHz, "fullscreenHz", 60);
SETTING(int, D3DAdapterOverride, "d3dAdapterOverride", -1);
SETTING(bool, PauseGame, "pauseGame", false);
