#include "Detouring.h"
#include "TextureManager.h"
#include "d3d9.h"
#include "log.h"
#include "main.h"
#include "tinyformat.h"
#include "util.h"
#include <MinHook.h>

decltype(Direct3DCreate9)* oDirect3DCreate9;

decltype(D3DXCreateTextureFromFileInMemory)* OrigD3DXCreateTextureFromFileInMemory;

HRESULT WINAPI DetouredD3DXCreateTextureFromFileInMemory(_In_ LPDIRECT3DDEVICE9 pDevice,
                                                         _In_ LPCVOID pSrcData,
                                                         _In_ UINT SrcDataSize,
                                                         _Out_ LPDIRECT3DTEXTURE9* ppTexture) {
  HRESULT res = OrigD3DXCreateTextureFromFileInMemory(pDevice, pSrcData, SrcDataSize, ppTexture);
  TextureManager::get().registerKnownTexture(pSrcData, SrcDataSize, *ppTexture);
  return res;
}

decltype(D3DXCreateTextureFromFileInMemoryEx)* OrigD3DXCreateTextureFromFileInMemoryEx;

HRESULT WINAPI DetouredD3DXCreateTextureFromFileInMemoryEx(
    LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height,
    UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter,
    D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
    LPDIRECT3DTEXTURE9* ppTexture) {
  HRESULT res = OrigD3DXCreateTextureFromFileInMemoryEx(
      pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter,
      MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
  TextureManager::get().registerKnownTexture(pSrcData, SrcDataSize, *ppTexture);
  return res;
}

void* hookFunction(const char* pFunctionName, const wchar_t* pModuleName, void* const pDetour,
                   void** ppOriginal) {
  HMODULE hModule = GetModuleHandleW(pModuleName);
  void* pTarget = reinterpret_cast<void*>(::GetProcAddress(hModule, pFunctionName));
  MH_CreateHook(pTarget, pDetour, ppOriginal);
  MH_EnableHook(pTarget);
  return pTarget;
}

namespace {
void* Direct3DCreate9Handle;
void* D3DXCreateTextureFromFileInMemoryHandle;
void* D3DXCreateTextureFromFileInMemoryExHandle;
} // namespace

void hookDirect3DCreate9() {
  try {
    Direct3DCreate9Handle = hookFunction("Direct3DCreate9", L"d3d9.dll", (void*)&hkDirect3DCreate9,
                                         (void**)&oDirect3DCreate9);
  } catch (const std::runtime_error& exp) {
    SDLOG(LogLevel::Error, exp.what());
  }
}

void startDetour() {
  try {
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
  MH_RemoveHook(Direct3DCreate9Handle);
  MH_RemoveHook(D3DXCreateTextureFromFileInMemoryHandle);
  MH_RemoveHook(D3DXCreateTextureFromFileInMemoryExHandle);
}
