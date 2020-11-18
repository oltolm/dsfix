#pragma once
#include "Effect.h"
#include <d3d9.h>
class SSAO : public Effect {
public:
  enum Type { VSSAO, VSSAO2 };
  SSAO(IDirect3DDevice9* device, int width, int height, unsigned strength, Type type) noexcept;
  virtual ~SSAO() = default;
  void go(IDirect3DTexture9* frame, IDirect3DTexture9* depth, IDirect3DSurface9* dst) noexcept;

private:
  int width, height;
  ID3DXEffectPtr effect;
  IDirect3DTexture9Ptr buffer1Tex;
  IDirect3DSurface9Ptr buffer1Surf;
  IDirect3DTexture9Ptr buffer2Tex;
  IDirect3DSurface9Ptr buffer2Surf;
  D3DXHANDLE depthTexHandle, frameTexHandle, prevPassTexHandle;
  void mainSsaoPass(IDirect3DTexture9* depth, IDirect3DSurface9* dst);
  void vBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
  void hBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
  void combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst);
};
