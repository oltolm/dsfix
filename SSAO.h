#pragma once
#include "Effect.h"
#include <d3dx9.h>
#include <wrl.h>

using namespace Microsoft;

class SSAO : public Effect {
public:
  enum class Type { HBAO, VSSAO, VSSAO2 };
  SSAO(IDirect3DDevice9* device, unsigned int width, unsigned int height, unsigned strength, Type type) noexcept;
  virtual ~SSAO() = default;
  void go(IDirect3DTexture9* frame, IDirect3DTexture9* depth, IDirect3DSurface9* dst) noexcept;

private:
  int width, height;
  Microsoft::WRL::ComPtr<ID3DXEffect> effect;
  Microsoft::WRL::ComPtr<IDirect3DTexture9> buffer1Tex;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> buffer1Surf;
  Microsoft::WRL::ComPtr<IDirect3DTexture9> buffer2Tex;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> buffer2Surf;
  D3DXHANDLE depthTexHandle, frameTexHandle, prevPassTexHandle;
  void mainSsaoPass(IDirect3DTexture9* depth, IDirect3DSurface9* dst);
  void vBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
  void hBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
  void combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst);
};
