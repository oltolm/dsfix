#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include "main.h"
#include "Detouring.h"
#include "FPS.h"
#include "KeyActions.h"
#include "Settings.h"
#include "log.h"
#include "tinyformat.h"
#include "util.h"
#include <MinHook.h>
#include <d3d9.h>
#include <filesystem>
#include <fstream>
#include <windows.h>

namespace fs = std::filesystem;

DirectInput8Create_t oDirectInput8Create;

namespace dsfix {
std::ofstream log;
}

void loadOriginalDinput8dll() {
  HMODULE hMod;
  auto wrapper = Settings::get().getDinput8dllWrapper();
  if (wrapper.empty() || (wrapper.find(L"none") == 0)) {
    fs::path dinput8Filename = GetSystemDirectoryPath() / L"dinput8.dll";
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

void onDirect3D9Create() {
  static bool initialized = false;
  if (initialized)
    return;
  fs::path logFilename = GetModuleDirectoryPath() / L"DSfix.log";
  dsfix::log.open(logFilename);
  Settings::get().load();
  Settings::get().report();
  KeyActions::get().load();
  KeyActions::get().report();
  SDLOG(LogLevel::Info, "===== start DSfix %s = fn: %s", VERSION, GetModuleFileNamePath(nullptr));
  loadOriginalDinput8dll();
  initFPSTimer();
  if (Settings::get().getUnlockFPS())
    applyFPSPatch();
  startDetour();
  initialized = true;
  SDLOG(LogLevel::Debug, "onDirect3D9Create finished");
}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, PVOID pvReserved __attribute__((unused))) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    ::DisableThreadLibraryCalls(hDll);
    MH_Initialize();
    hookDirect3DCreate9();
    return TRUE;
  } else if (dwReason == DLL_PROCESS_DETACH) {
    SDLOG(LogLevel::Info, "shutting down");
    Settings::get().shutdown();
    endDetour();
    MH_Uninitialize();
  }
  return FALSE;
}

void sdlog(const char* fmt, tfm::FormatListRef formatList) {
  if (dsfix::log.good()) {
    tfm::vformat(dsfix::log, fmt, formatList);
    dsfix::log << std::endl;
  }
}
