#pragma once
#include "Effect.h"
#include <d3d9.h>
class FXAA : public Effect {
public:
  enum Quality { QualityLow, QualityMedium, QualityHigh, QualityUltra };
  FXAA(IDirect3DDevice9* device, int width, int height, Quality quality) noexcept;
  virtual ~FXAA() = default;
  void go(IDirect3DTexture9* frame, IDirect3DSurface9* dst) noexcept;

private:
  int width, height;
  ID3DXEffectPtr effect;
  IDirect3DTexture9Ptr buffer1Tex;
  IDirect3DSurface9Ptr buffer1Surf;
  D3DXHANDLE frameTexHandle;
  void lumaPass(IDirect3DTexture9* frame, IDirect3DSurface9* dst);
  void fxaaPass(IDirect3DTexture9* src, IDirect3DSurface9* dst);
};
