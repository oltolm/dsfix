#pragma once
#include "Effect.h"
#include <wrl.h>

class GAUSS : public Effect {
public:
  GAUSS(IDirect3DDevice9* device, int width, int height) noexcept;
  virtual ~GAUSS() = default;
  void go(IDirect3DTexture9* input, IDirect3DSurface9* dst) noexcept;

private:
  int width, height;
  Microsoft::WRL::ComPtr<ID3DXEffect> effect;
  Microsoft::WRL::ComPtr<IDirect3DTexture9> buffer1Tex;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> buffer1Surf;
  D3DXHANDLE frameTexHandle;
};
