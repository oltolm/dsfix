#pragma once
#include "FXAA.h"
#include "GAUSS.h"
#include "SMAA/SMAA.h"
#include "SSAO.h"
#include <array>
#include <memory>
#include <spdlog/spdlog.h>
#include <wrl.h>

class RSManager {
private:
  D3DVIEWPORT9 viewport = {};
  Microsoft::WRL::ComPtr<IDirect3DDevice9> m_pDevice;
  double lastPresentTime = 0;
  std::unique_ptr<SMAA> smaa;
  std::unique_ptr<FXAA> fxaa;
  std::unique_ptr<SSAO> ssao;
  std::unique_ptr<GAUSS> gauss;
  Microsoft::WRL::ComPtr<IDirect3DTexture9> rgbaBuffer1Tex;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> rgbaBuffer1Surf;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> depthStencilSurf;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> zSurf;
  // NumRenderTargetSwitches
  // we use the number of switches between rendertargets to figure out where we are
  // in the pipeline. Yeah, it's flaky
  unsigned int nrts = 0;
  // Count the number of times the 2 upper DoF rendertargets were set in doft[1]
  // & doft[2]
  std::array<unsigned int, 3> doft = {};
  // main rendertarget for this frame
  Microsoft::WRL::ComPtr<IDirect3DSurface9> mainRT;
  unsigned int mainRTuses = 0;
  Microsoft::WRL::ComPtr<IDirect3DVertexDeclaration9> prevVDecl;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> prevDepthStencilSurf;
  Microsoft::WRL::ComPtr<IDirect3DStateBlock9> prevStateBlock;
  bool haveOcclusionScale = false;
  float occlusionScale = 1;
  unsigned int isDof(unsigned int width, unsigned int height);
  void measureOcclusionScale();
  void frameTimeManagement();

public:
  ~RSManager() = default;
  RSManager(IDirect3DDevice9* pDevice);

  void setupAA();
  void setupSSAO();
  void setupDoF();
  void onReset();

  void setViewport(const D3DVIEWPORT9& vp) { viewport = vp; }

  bool isViewport(const RECT& r) {
    return (r.left == static_cast<LONG>(viewport.X)) && (r.top == static_cast<LONG>(viewport.Y)) &&
           (r.bottom == static_cast<LONG>(viewport.Height)) &&
           (r.right == static_cast<LONG>(viewport.Width));
  }

  HRESULT redirectSetRenderTarget(DWORD RenderTargetIndex,
                                  IDirect3DSurface9* pRenderTarget) noexcept;
  HRESULT redirectPresent(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride,
                          CONST RGNDATA* pDirtyRegion) noexcept;

  float getOcclusionScale() const { return occlusionScale; }

  // Render state store/restore
  void storeRenderState();
  void restoreRenderState();
};
