#pragma once
#include "comptr.h"
#include <vector>
#include <windows.h>

class TextureManager {
private:
  static TextureManager instance;
  unsigned numKnownTextures = 0, foundKnownTextures = 0;

  void registerKnownTexture(LPCVOID pSrcData, UINT SrcDataSize,
                            LPDIRECT3DTEXTURE9 pTexture) noexcept;

public:
  TextureManager() {
#define TEXTURE(_name, _hash) ++numKnownTextures;
#include "Textures.inc"
#undef TEXTURE
  }
#define TEXTURE(_name, _hash)                                                                      \
  static const UINT32 texture##_name##Hash = _hash;                                                \
  IDirect3DTexture9Ptr texture##_name;                                                             \
  bool isTexture##_name(IDirect3DBaseTexture9* pTexture) {                                         \
    return texture##_name && ((IDirect3DTexture9*)pTexture) == texture##_name;                     \
  };
#include "Textures.inc"
#undef TEXTURE
  static TextureManager& get() { return instance; }
  void registerD3DXCreateTextureFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize,
                                                 LPDIRECT3DTEXTURE9 pTexture) noexcept;
  HRESULT redirectD3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData,
                                                      UINT SrcDataSize, UINT Width, UINT Height,
                                                      UINT MipLevels, DWORD Usage, D3DFORMAT Format,
                                                      D3DPOOL Pool, DWORD Filter, DWORD MipFilter,
                                                      D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo,
                                                      PALETTEENTRY* pPalette,
                                                      LPDIRECT3DTEXTURE9* ppTexture);
  bool isTextureText(IDirect3DBaseTexture9* t);
  const char* getTextureName(IDirect3DBaseTexture9* pTexture);
};
