#include "TextureManager.h"
#include <filesystem>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

UINT32 SuperFastHash(const char* data, int len);

TextureManager TextureManager::instance;

void TextureManager::registerKnownTexture(LPCVOID pSrcData, UINT SrcDataSize,
                                          LPDIRECT3DTEXTURE9 pTexture) noexcept {
  if (foundKnownTextures < numKnownTextures) {
    UINT32 hash = SuperFastHash(static_cast<const char*>(pSrcData), SrcDataSize);
#define TEXTURE(_name, _hash)                                                                      \
  if (hash == _hash) {                                                                             \
    texture##_name = pTexture;                                                                     \
    ++foundKnownTextures;                                                                          \
    spdlog::info("TextureManager: recognized known texture {} at {:p}", #_name, (void*)pTexture);  \
  }
#include "Textures.inc"
#undef TEXTURE
    if (foundKnownTextures == numKnownTextures) {
      spdlog::info("TextureManager: all known textures found!");
    }
  }
}

bool TextureManager::isTextureText(IDirect3DBaseTexture9* t) {
  return isTextureText00(t) || isTextureText01(t) || isTextureText02(t) || isTextureText03(t) ||
         isTextureText04(t) || isTextureText05(t) || isTextureText06(t) || isTextureText07(t) ||
         isTextureText08(t) || isTextureText09(t) || isTextureText10(t) || isTextureText11(t) ||
         isTextureText12(t);
}
