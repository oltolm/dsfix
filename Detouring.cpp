#include "Detouring.h"
#include "Settings.h"
#include "d3d9.h"
#include <MinHook.h>
#include <spdlog/spdlog.h>

decltype(Direct3DCreate9)* oDirect3DCreate9;
decltype(GetUserDefaultLangID)* oGetUserDefaultLangID;

namespace {
void* Direct3DCreate9Handle;
void* GetUserDefaultLangIDHandle;
} // namespace

// en-GB = English, fr = French, it = Italian, de = German, es = Spanish
// ko = Korean, zh-tw = Chinese, pl = Polish, ru = Russian
LANGID hkGetUserDefaultLangID() {
  const auto& lang = Settings::get().getOverrideLanguage();
  if (lang == "en-GB")
    return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK);
  else if (lang == "fr")
    return MAKELANGID(LANG_FRENCH, SUBLANG_DEFAULT);
  else if (lang == "it")
    return MAKELANGID(LANG_ITALIAN, SUBLANG_DEFAULT);
  else if (lang == "de")
    return MAKELANGID(LANG_GERMAN, SUBLANG_DEFAULT);
  else if (lang == "es")
    return MAKELANGID(LANG_SPANISH, SUBLANG_DEFAULT);
  else if (lang == "ko")
    return MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT);
  else if (lang == "zh-tw")
    return MAKELANGID(LANG_CHINESE_TRADITIONAL, SUBLANG_DEFAULT);
  else if (lang == "pl")
    return MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT);
  else if (lang == "ru")
    return MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
  else
    return oGetUserDefaultLangID();
}

void startDetour() {
  MH_Initialize();
  Direct3DCreate9Handle =
      reinterpret_cast<void*>(::GetProcAddress(GetModuleHandleW(L"d3d9.dll"), "Direct3DCreate9"));
  MH_CreateHook(Direct3DCreate9Handle, reinterpret_cast<void*>(&hkDirect3DCreate9),
                reinterpret_cast<void**>(&oDirect3DCreate9));
  MH_EnableHook(Direct3DCreate9Handle);
  GetUserDefaultLangIDHandle = reinterpret_cast<void*>(
      ::GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetUserDefaultLangID"));
  MH_CreateHook(GetUserDefaultLangIDHandle, reinterpret_cast<void*>(&hkGetUserDefaultLangID),
                reinterpret_cast<void**>(&oGetUserDefaultLangID));
  MH_EnableHook(GetUserDefaultLangIDHandle);
}

void endDetour() {
  MH_RemoveHook(Direct3DCreate9Handle);
  MH_Uninitialize();
}
