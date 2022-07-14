#pragma once
#include "FXAA.h"
#include "GAUSS.h"
#include "SMAA.h"
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
  bool doAA = true;
  std::unique_ptr<SMAA> smaa;
  std::unique_ptr<FXAA> fxaa;
  bool doSsao = true;
  std::unique_ptr<SSAO> ssao;
  bool doDofGauss = true;
  std::unique_ptr<GAUSS> gauss;
  Microsoft::WRL::ComPtr<IDirect3DTexture9> rgbaBuffer1Tex;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> rgbaBuffer1Surf;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> depthStencilSurf;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> zSurf;
  // RenderDoneDetectionProgress
  // basically, when the game switches rendertargets after setting texture 0 to 3
  // in order, but no others, we assume we just finished rendering the hud-less
  // image. This variable keeps track of the number of "correct" texture
  // settings.
  unsigned int rddp = 0;
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

  void toggleSsao() { doSsao = !doSsao; }

  void toggleAA() { doAA = !doAA; }

  void toggleDofGauss() { doDofGauss = !doDofGauss; }

  HRESULT redirectSetRenderTarget(DWORD RenderTargetIndex,
                                  IDirect3DSurface9* pRenderTarget) noexcept;
  HRESULT redirectSetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) noexcept;
  HRESULT redirectPresent(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride,
                          CONST RGNDATA* pDirtyRegion) noexcept;

  float getOcclusionScale() const { return occlusionScale; }

  // Render state store/restore
  void storeRenderState();
  void restoreRenderState();
};
