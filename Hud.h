#pragma once
#include "Effect.h"
class HUD : public Effect {
public:
  HUD(IDirect3DDevice9* device, int width, int height);
  virtual ~HUD() = default;
  void go(IDirect3DTexture9* input, IDirect3DSurface9* dst);

private:
  int width, height;
  ID3DXEffectPtr effect;
  D3DXHANDLE frameTexHandle;
  D3DXHANDLE opacityHandle;
  void rect(float srcLeft, float srcTop, float srcWidth, float srcHeight, float trgLeft,
            float trgTop, float trgWidth, float trgHeight);
};
