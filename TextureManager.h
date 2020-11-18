#pragma once
#include <windows.h>
#include <d3dx9.h>
#include <wrl.h>

class TextureManager {
private:
  static TextureManager instance;

public:
  TextureManager() = default;
#define TEXTURE(_name, _hash)                                                                      \
  static const UINT32 texture##_name##Hash = _hash;                                                \
  Microsoft::WRL::ComPtr<IDirect3DTexture9> texture##_name;                                        \
  bool isTexture##_name(IDirect3DBaseTexture9* pTexture) {                                         \
    return texture##_name && ((IDirect3DTexture9*)pTexture) == texture##_name.Get();               \
  };
#include "Textures.inc"
#undef TEXTURE
  static TextureManager& get() { return instance; }
  bool isTextureText(IDirect3DBaseTexture9* t);
};
