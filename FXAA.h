#pragma once
#include "../Effect.h"
#include <wrl/client.h>

class FXAA : public Effect {
public:
  enum Quality { QualityLow, QualityMedium, QualityHigh, QualityUltra };

  FXAA(IDirect3DDevice9* device, int width, int height, Quality quality) noexcept;
  virtual ~FXAA() = default;
  void go(IDirect3DTexture9* frame, IDirect3DSurface9* dst) noexcept;

private:
  int width, height;
  Microsoft::WRL::ComPtr<ID3DXEffect> effect;
  Microsoft::WRL::ComPtr<IDirect3DTexture9> buffer1Tex;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> buffer1Surf;
  D3DXHANDLE frameTexHandle;
  void lumaPass(IDirect3DTexture9* frame, IDirect3DSurface9* dst);
  void fxaaPass(IDirect3DTexture9* src, IDirect3DSurface9* dst);
};
