#pragma once
#include "Effect.h"
#include <d3d9.h>
class GAUSS : public Effect {
public:
  GAUSS(IDirect3DDevice9* device, int width, int height) noexcept;
  virtual ~GAUSS() = default;
  void go(IDirect3DTexture9* input, IDirect3DSurface9* dst) noexcept;

private:
  int width, height;
  ID3DXEffectPtr effect;
  IDirect3DTexture9Ptr buffer1Tex;
  IDirect3DSurface9Ptr buffer1Surf;
  D3DXHANDLE frameTexHandle;
};
