#include "Detouring.h"
#include "d3d9.h"
#include <MinHook.h>
#include <spdlog/spdlog.h>

decltype(Direct3DCreate9)* oDirect3DCreate9;

namespace {
void* Direct3DCreate9Handle;
} // namespace

void startDetour() {
  MH_Initialize();
  Direct3DCreate9Handle =
      reinterpret_cast<void*>(::GetProcAddress(GetModuleHandleW(L"d3d9.dll"), "Direct3DCreate9"));
  MH_CreateHook(Direct3DCreate9Handle, reinterpret_cast<void*>(&hkDirect3DCreate9),
                reinterpret_cast<void**>(&oDirect3DCreate9));
  MH_EnableHook(Direct3DCreate9Handle);
}

void endDetour() {
  MH_RemoveHook(Direct3DCreate9Handle);
  MH_Uninitialize();
}
