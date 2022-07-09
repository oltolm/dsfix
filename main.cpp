#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include "main.h"
#include "Detouring.h"
#include "FPS.h"
#include "Settings.h"
#include "util.h"
#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <windows.h>
#include "dinput.h"
#include "ui.h"
#include "RenderstateManager.h"
#include "d3d9int.h"
#include "d3d9dev.h"

namespace fs = std::filesystem;

void loadOriginalDinput8dll() {
  fs::path dinput8Filename = GetSystemDirectoryPath() / L"dinput8.dll";
  HMODULE hMod = ::LoadLibraryW(dinput8Filename.c_str());
  if (!hMod) {
    spdlog::debug(L"Could not load original dinput8.dll");
    spdlog::debug(L"ABORTING.");
    std::exit(1);
  }
  oDirectInput8Create = (decltype(DirectInput8Create)*)::GetProcAddress(hMod, "DirectInput8Create");
}

void onDllProcessAttach() {
  auto logger = spdlog::basic_logger_st("dsfix", GetModuleDirectoryPath() / L"dsfix.log", true);
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  Settings::get().load();
  Settings::get().report();
  loadOriginalDinput8dll();
  startDetour();
  spdlog::set_level(static_cast<spdlog::level::level_enum>(Settings::get().getLogLevel()));
  spdlog::info(L"===== start DSfix {} = fn: {}", DXVK_VERSION, GetModuleFileNamePath(nullptr));
  if (Settings::get().getSkipIntro())
    ApplyDS1Patches();
}

void onDllProcessDetach() {
  spdlog::info("shutting down");
  Settings::get().shutdown();
  removeFPSHook();
  endDetour();
}

// here the dependency injection (DI) happens
void onD3DCreateDevice() {
  static bool s_initialized = false;
  if (s_initialized)
    return;

  RSManager::get().setD3DDevice(g_pD3DDevice->getDevice());
  Ui::get().setD3DDevice(g_pD3DDevice.Get());

  initFPSTimer();
  if (Settings::get().getUnlockFPS())
    applyFPSPatch();
  s_initialized = true;
  spdlog::debug("onDirect3D9Create finished");
}

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter) {
  onDllProcessAttach();
  return ERROR_SUCCESS;
}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, PVOID pvReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    ::DisableThreadLibraryCalls(hDll);
    ::CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    return TRUE;
  } else if (dwReason == DLL_PROCESS_DETACH) {
    // onDllProcessDetach();
  }
  return FALSE;
}
