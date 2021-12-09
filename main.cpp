#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include "main.h"
#include "Detouring.h"
#include "FPS.h"
#include "KeyActions.h"
#include "Settings.h"
#include "util.h"
#include <MinHook.h>
#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <windows.h>

namespace fs = std::filesystem;

DirectInput8Create_t oDirectInput8Create;

void loadOriginalDinput8dll() {
  HMODULE hMod;
  auto wrapper = Settings::get().getDinput8dllWrapper();
  if (wrapper.empty() || (wrapper.find(L"none") == 0)) {
    fs::path dinput8Filename = GetSystemDirectoryPath() / L"dinput8.dll";
    hMod = ::LoadLibraryW(dinput8Filename.c_str());
  } else {
    spdlog::debug(L"Loading dinput wrapper {}", wrapper);
    hMod = ::LoadLibraryW(wrapper.c_str());
  }
  if (!hMod) {
    spdlog::debug(L"Could not load original dinput8.dll");
    spdlog::debug(L"ABORTING.");
    spdlog::debug(L"Loading of specified dinput wrapper: {}", GetLastErrorString());
    exit(1);
  }
  oDirectInput8Create = (DirectInput8Create_t)::GetProcAddress(hMod, "DirectInput8Create");
}

void onDllProcessAttach() {
  auto logger = spdlog::basic_logger_st("dsfix", GetModuleDirectoryPath() / L"dsfix.log", true);
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  Settings::get().load();
  Settings::get().report();
  loadOriginalDinput8dll();
  MH_Initialize();
  hookDirect3DCreate9();
  spdlog::set_level(static_cast<spdlog::level::level_enum>(Settings::get().getLogLevel()));
  KeyActions::get().load();
  KeyActions::get().report();
  spdlog::info(L"===== start DSfix {} = fn: {}", VERSION, GetModuleFileNamePath(nullptr));
}

void onDllProcessDetach() {
  spdlog::info("shutting down");
  Settings::get().shutdown();
  removeFPSHook();
  endDetour();
  MH_Uninitialize();
}

void onDirect3D9Create() {
  static bool initialized = false;
  if (initialized)
    return;
  initFPSTimer();
  if (Settings::get().getUnlockFPS())
    applyFPSPatch();
  startDetour();
  initialized = true;
  spdlog::debug("onDirect3D9Create finished");
}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, PVOID pvReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    ::DisableThreadLibraryCalls(hDll);
    onDllProcessAttach();
    return TRUE;
  } else if (dwReason == DLL_PROCESS_DETACH) {
    onDllProcessDetach();
  }
  return FALSE;
}
