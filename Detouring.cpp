#include "Detouring.h"
#include "Settings.h"
#include "TextureManager.h"
#include "d3d9.h"
#include "log.h"
#include "main.h"
#include "tinyformat.h"
#include "util.h"
#include <MinHook.h>
#include <Windows.h>

bool timingIntroMode = false;
static LARGE_INTEGER perfCountIncrease, countsPerSec;

BOOL(WINAPI* OrigQueryPerformanceCounter)
(_Out_ LARGE_INTEGER* lpPerformanceCount) = QueryPerformanceCounter;

BOOL WINAPI DetouredQueryPerformanceCounter(_Out_ LARGE_INTEGER* lpPerformanceCount) {
  void* traces[128];
  int captured = CaptureStackBackTrace(0, 128, traces, nullptr);
  BOOL ret = OrigQueryPerformanceCounter(lpPerformanceCount);
  if (timingIntroMode && captured < 3) {
    perfCountIncrease.QuadPart += countsPerSec.QuadPart / 50;
  }
  lpPerformanceCount->QuadPart += perfCountIncrease.QuadPart;
  return ret;
}

D3DXCreateTexture_FNType OrigD3DXCreateTexture = D3DXCreateTexture;

HRESULT WINAPI DetouredD3DXCreateTexture(_In_ LPDIRECT3DDEVICE9 pDevice, _In_ UINT Width,
                                         _In_ UINT Height, _In_ UINT MipLevels, _In_ DWORD Usage,
                                         _In_ D3DFORMAT Format, _In_ D3DPOOL Pool,
                                         _Out_ LPDIRECT3DTEXTURE9* ppTexture) {
  return OrigD3DXCreateTexture(pDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
}

D3DXCreateTextureFromFileInMemory_FNType OrigD3DXCreateTextureFromFileInMemory =
    D3DXCreateTextureFromFileInMemory;

HRESULT WINAPI DetouredD3DXCreateTextureFromFileInMemory(_In_ LPDIRECT3DDEVICE9 pDevice,
                                                         _In_ LPCVOID pSrcData,
                                                         _In_ UINT SrcDataSize,
                                                         _Out_ LPDIRECT3DTEXTURE9* ppTexture) {
  HRESULT res = OrigD3DXCreateTextureFromFileInMemory(pDevice, pSrcData, SrcDataSize, ppTexture);
  TextureManager::get().registerD3DXCreateTextureFromFileInMemory(pSrcData, SrcDataSize,
                                                                  *ppTexture);
  return res;
}

D3DXCreateTextureFromFileInMemoryEx_FNType OrigD3DXCreateTextureFromFileInMemoryEx =
    D3DXCreateTextureFromFileInMemoryEx;

HRESULT WINAPI DetouredD3DXCreateTextureFromFileInMemoryEx(
    LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height,
    UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter,
    D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
    LPDIRECT3DTEXTURE9* ppTexture) {
  return TextureManager::get().redirectD3DXCreateTextureFromFileInMemoryEx(
      pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter,
      MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
}

void* hookFunction(const char* pFunctionName, const wchar_t* pModuleName, void* const pDetour,
                   void** ppOriginal) {
  HMODULE hModule = GetModuleHandleW(pModuleName);
  throw_if_null(hModule);
  void* pTarget = reinterpret_cast<void*>(::GetProcAddress(hModule, pFunctionName));
  throw_if_null(pTarget);
  MH_STATUS ret = MH_CreateHook(pTarget, pDetour, ppOriginal);
  if (ret != MH_OK)
    throw std::runtime_error(tfm::format("hooking %s in %s failed: %s", pFunctionName, pModuleName,
                                         MH_StatusToString(ret)));
  SDLOG(LogLevel::Info, "hooking %s in %s succeeded", pFunctionName, pModuleName);
  ret = MH_EnableHook(pTarget);
  if (ret != MH_OK)
    throw std::runtime_error(tfm::format("MH_EnableHook failed: %s", MH_StatusToString(ret)));
  return pTarget;
}

namespace {
void* Direct3DCreate9Handle;
void* QueryPerformanceCounterHandle;
void* D3DXCreateTextureFromFileInMemoryHandle;
void* D3DXCreateTextureFromFileInMemoryExHandle;
} // namespace
void earlyDetour() {
  QueryPerformanceFrequency(&countsPerSec);
  try {
    MH_Initialize();
    Direct3DCreate9Handle = hookFunction("Direct3DCreate9", L"d3d9.dll", (void*)&hkDirect3DCreate9,
                                         (void**)&oDirect3DCreate9);
  } catch (const std::runtime_error& exp) {
    SDLOG(LogLevel::Error, exp.what());
  }
}

void startDetour() {
  try {
    if (Settings::get().getSkipIntro()) {
      QueryPerformanceCounterHandle = hookFunction("QueryPerformanceCounter", L"kernel32.dll",
                                                   (void*)&DetouredQueryPerformanceCounter,
                                                   (void**)&OrigQueryPerformanceCounter);
    }
    D3DXCreateTextureFromFileInMemoryHandle =
        hookFunction("D3DXCreateTextureFromFileInMemory", L"d3dx9_43.dll",
                     (void*)&DetouredD3DXCreateTextureFromFileInMemory,
                     (void**)&OrigD3DXCreateTextureFromFileInMemory);
    D3DXCreateTextureFromFileInMemoryExHandle =
        hookFunction("D3DXCreateTextureFromFileInMemoryEx", L"d3dx9_43.dll",
                     (void*)&DetouredD3DXCreateTextureFromFileInMemoryEx,
                     (void**)&OrigD3DXCreateTextureFromFileInMemoryEx);
  } catch (const std::runtime_error& exp) {
    SDLOG(LogLevel::Error, exp.what());
  }
}

void endDetour() {
  MH_STATUS res = MH_RemoveHook(Direct3DCreate9Handle);
  if (res != MH_OK)
    SDLOG(LogLevel::Error, "MH_RemoveHook failed %s", MH_StatusToString(res));
  res = MH_RemoveHook(QueryPerformanceCounterHandle);
  if (res != MH_OK)
    SDLOG(LogLevel::Error, "MH_RemoveHook failed %s", MH_StatusToString(res));
  res = MH_RemoveHook(D3DXCreateTextureFromFileInMemoryHandle);
  if (res != MH_OK)
    SDLOG(LogLevel::Error, "MH_RemoveHook failed %s", MH_StatusToString(res));
  res = MH_RemoveHook(D3DXCreateTextureFromFileInMemoryExHandle);
  if (res != MH_OK)
    SDLOG(LogLevel::Error, "MH_RemoveHook failed %s", MH_StatusToString(res));
  res = MH_Uninitialize();
  if (res != MH_OK)
    SDLOG(LogLevel::Error, "MH_Uninitialize failed %s", MH_StatusToString(res));
}
