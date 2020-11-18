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
    if (Settings::get().getEnableTextureDumping()) {
      UINT32 hash = SuperFastHash(static_cast<const char*>(pSrcData), SrcDataSize);
      SDLOG(LogLevel::Debug, " - size: %8u, hash: %8x", SrcDataSize, hash);
      IDirect3DSurface9* surf;
      throw_if_fail(pTexture->GetSurfaceLevel(0, &surf));
      fs::path destfile =
          GetModuleDirectoryPath() / "dsfix\\tex_dump" / tfm::format("%08x.dds", hash);
      auto Function = [](LPVOID lpThreadParameter) WINAPI -> DWORD {
        auto context = static_cast<std::pair<fs::path, IDirect3DSurface9*>*>(lpThreadParameter);
        ::D3DXSaveSurfaceToFileW(context->first.c_str(), D3DXIFF_DDS, context->second, NULL, NULL);
        context->second->Release();
        delete context;
        return 0;
      };
      auto Context = new std::pair(destfile, surf);
      ::QueueUserWorkItem(Function, static_cast<LPVOID>(Context), WT_EXECUTEINPERSISTENTTHREAD);
    }

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
  if (Settings::get().getEnableTextureOverride()) {
    UINT32 hash = SuperFastHash(reinterpret_cast<const char*>(pSrcData), SrcDataSize);
    SDLOG(LogLevel::Trace, "Trying texture override size: %8u, hash: %8x", SrcDataSize, hash);

    fs::path png = GetModuleDirectoryPath() / "dsfix\\tex_override" / tfm::format("%08x.png", hash);
    if (std::ifstream(png)) {
      SDLOG(LogLevel::Debug, "Texture override (png)! hash: %08x", hash);
      return ::D3DXCreateTextureFromFileExW(pDevice, png.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
                                            MipLevels, Usage, D3DFMT_FROM_FILE, Pool, Filter,
                                            MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
    }
    fs::path dds = GetModuleDirectoryPath() / "dsfix\\tex_override" / tfm::format("%08x.dds", hash);
    if (std::ifstream(dds)) {
      SDLOG(LogLevel::Debug, "Texture override (dds)! hash: %08x", hash);
      return ::D3DXCreateTextureFromFileExW(pDevice, dds.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
                                            MipLevels, Usage, D3DFMT_FROM_FILE, Pool, Filter,
                                            MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
    }
  }
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

const char* TextureManager::getTextureName(IDirect3DBaseTexture9* pTexture) {
#define TEXTURE(_name, _hash)                                                                      \
  if (texture##_name == pTexture)                                                                  \
    return #_name;
#include "Textures.inc"
#undef TEXTURE
  return "Unknown";
}
