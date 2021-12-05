#include "TextureManager.h"
#include "Detouring.h"
#include "FPS.h"
#include "log.h"
#include "util.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <threadpoollegacyapiset.h>

namespace fs = std::filesystem;

UINT32 SuperFastHash(const char* data, int len);

TextureManager TextureManager::instance;

void TextureManager::registerD3DXCreateTextureFromFileInMemory(
    LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9 pTexture) noexcept {
  SDLOG(LogLevel::Trace, "RenderstateManager: registerD3DXCreateTextureFromFileInMemory %p",
        pTexture);
  try {
    registerKnownTexture(pSrcData, SrcDataSize, pTexture);
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}

HRESULT TextureManager::redirectD3DXCreateTextureFromFileInMemoryEx(
    LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height,
    UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter,
    D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
    LPDIRECT3DTEXTURE9* ppTexture) {
  HRESULT res = OrigD3DXCreateTextureFromFileInMemoryEx(
      pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter,
      MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
  registerD3DXCreateTextureFromFileInMemory(pSrcData, SrcDataSize, *ppTexture);
  return res;
}

void TextureManager::registerKnownTexture(LPCVOID pSrcData, UINT SrcDataSize,
                                          LPDIRECT3DTEXTURE9 pTexture) noexcept {
  if (foundKnownTextures < numKnownTextures) {
    UINT32 hash = SuperFastHash(static_cast<const char*>(pSrcData), SrcDataSize);
#define TEXTURE(_name, _hash)                                                                      \
  if (hash == _hash) {                                                                             \
    texture##_name = pTexture;                                                                     \
    ++foundKnownTextures;                                                                          \
    SDLOG(LogLevel::Info, "TextureManager: recognized known texture %s at %p", #_name, pTexture);  \
  }
#include "Textures.inc"
#undef TEXTURE
    if (foundKnownTextures == numKnownTextures) {
      SDLOG(LogLevel::Info, "TextureManager: all known textures found!");
    }
  }
}

bool TextureManager::isTextureText(IDirect3DBaseTexture9* t) {
  return isTextureText00(t) || isTextureText01(t) || isTextureText02(t) || isTextureText03(t) ||
         isTextureText04(t) || isTextureText05(t) || isTextureText06(t) || isTextureText07(t) ||
         isTextureText08(t) || isTextureText09(t) || isTextureText10(t) || isTextureText11(t) ||
         isTextureText12(t);
}
