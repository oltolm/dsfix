// xmacro file for .ini setting definition
// format: (type, cname, ininame, default value)

// Graphics Options
SETTING(unsigned int, FilteringOverride, L"filteringOverride", 2);
SETTING(unsigned int, RenderWidth, L"renderWidth", 1920);
SETTING(unsigned int, RenderHeight, L"renderHeight", 1080);

SETTING(unsigned int, DOFOverrideResolution, L"dofOverrideResolution", 1079);
SETTING(unsigned int, DOFBlurAmount, L"dofBlurAmount", 1);

SETTING(unsigned int, SsaoStrength, L"ssaoStrength", 3);
SETTING(unsigned int, AAQuality, L"aaQuality", 4);
SETTING(std::wstring, AAType, L"aaType", L"FXAA");

SETTING(std::wstring, SsaoType, L"ssaoType", L"VSSAO2");

SETTING(bool, UnlockFPS, L"unlockFPS", true);
SETTING(unsigned int, FPSLimit, L"FPSlimit", 60);

// Logging Options
SETTING(unsigned int, LogLevel, L"logLevel", 0);

// Cursor Options
SETTING(bool, DisableCursor, L"disableCursor", false);
SETTING(bool, CaptureCursor, L"captureCursor", false);

// Skipping Options
SETTING(bool, SkipIntro, L"skipIntro", false);

// Screen Options
SETTING(bool, BorderlessFullscreen, L"borderlessFullscreen", false);
SETTING(unsigned int, PresentWidth, L"presentWidth", 0);
SETTING(unsigned int, PresentHeight, L"presentHeight", 0);
SETTING(bool, EnableVsync, L"enableVsync", false);
SETTING(unsigned int, FullscreenHz, L"fullscreenHz", 60);
SETTING(int, D3DAdapterOverride, L"d3dAdapterOverride", -1);
