#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include "main.h"
#include "Detouring.h"
#include "FPS.h"
#include "KeyActions.h"
#include "Settings.h"
#include "TextureManager.h"
#include "log.h"
#include "tinyformat.h"
#include "util.h"
#include <ctime>
#include <d3d9.h>
#include <fstream>
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

Direct3DCreate9_t oDirect3DCreate9 = Direct3DCreate9;
DirectInput8Create_t oDirectInput8Create;

namespace dsfix {
std::ofstream log;
}

void loadOriginalDinput8dll() {
  HMODULE hMod;
  auto wrapper = Settings::get().getDinput8dllWrapper();
  fs::path dinput8Filename = GetSystemDirectoryPath() / L"dinput8.dll";
  if (wrapper.empty() || (wrapper.find(L"none") == 0)) {
    hMod = ::LoadLibraryW(dinput8Filename.c_str());
  } else {
    tfm::format(dsfix::log, "Loading dinput wrapper %s\n", wrapper);
    hMod = ::LoadLibraryW(wrapper.c_str());
  }
  if (!hMod) {
    tfm::format(dsfix::log, "Could not load original dinput8.dll\n");
    tfm::format(dsfix::log, "ABORTING.\n");
    tfm::format(dsfix::log, "Loading of specified dinput wrapper: %s\n", GetLastErrorString());
    exit(1);
  }
  oDirectInput8Create = (DirectInput8Create_t)::GetProcAddress(hMod, "DirectInput8Create");
}

void init() {
  fs::path logFilename = GetModuleDirectoryPath() / L"DSfix.log";
  dsfix::log.open(logFilename);
  Settings::get().load();
  Settings::get().report();
  KeyActions::get().load();
  KeyActions::get().report();
  std::time_t now = std::time(nullptr);
  SDLOG(LogLevel::Info, "===== %.24s =====", std::ctime(&now));
  SDLOG(LogLevel::Info, "===== start DSfix %s = fn: %s", VERSION, GetModuleFileNamePath(NULL));
  loadOriginalDinput8dll();
  initFPSTimer();
  if (Settings::get().getUnlockFPS())
    applyFPSPatch();
}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, PVOID pvReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    ::DisableThreadLibraryCalls(hDll);
    earlyDetour();
    return TRUE;
  } else if (dwReason == DLL_PROCESS_DETACH) {
    SDLOG(LogLevel::Info, "shutting down");
    Settings::get().shutdown();
    endDetour();
  }
  return FALSE;
}

void sdlog(const char* fmt, tfm::FormatListRef formatList) {
  if (dsfix::log.good()) {
    tfm::vformat(dsfix::log, fmt, formatList);
    dsfix::log << std::endl;
  }
}
