#include "TextureManager.h"

TextureManager TextureManager::instance;

bool TextureManager::isTextureText(IDirect3DBaseTexture9* t) {
  return isTextureText00(t) || isTextureText01(t) || isTextureText02(t) || isTextureText03(t) ||
         isTextureText04(t) || isTextureText05(t) || isTextureText06(t) || isTextureText07(t) ||
         isTextureText08(t) || isTextureText09(t) || isTextureText10(t) || isTextureText11(t) ||
         isTextureText12(t);
}
