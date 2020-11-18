#pragma once
#include "FXAA.h"
#include "GAUSS.h"
#include "HUD.h"
#include "SMAA.h"
#include "SSAO.h"
#include "comptr.h"
#include <memory>

class RSManager {
private:
  static RSManager instance;
  bool initialized = false;
  D3DVIEWPORT9 viewport;
  IDirect3DDevice9Ptr d3ddev;
  double lastPresentTime = 0;
  bool doAA = false;
  std::unique_ptr<SMAA> smaa;
  std::unique_ptr<FXAA> fxaa;
  bool doSsao = true;
  std::unique_ptr<SSAO> ssao;
  bool doDofGauss = true;
  std::unique_ptr<GAUSS> gauss;
  bool doHud = true;
  std::unique_ptr<HUD> hud;
  IDirect3DTexture9Ptr rgbaBuffer1Tex;
  IDirect3DSurface9Ptr rgbaBuffer1Surf;
  IDirect3DSurface9Ptr depthStencilSurf;
  IDirect3DSurface9Ptr zSurf;
  bool hideHud = false;
  bool onHudRT = false;
  bool pausedHudRT = false;
  bool paused = false;
  bool hudStarted = false;
  bool takeScreenshot = false;
  unsigned skippedPresents = 0;
  // RenderDoneDetectionProgress
  // basically, when the game switches rendertargets after setting texture 0 to 3
  // in order, but no others, we assume we just finished rendering the hud-less
  // image. This variable keeps track of the number of "correct" texture
  // settings.
  unsigned rddp = 0;
  // NumRenderTargetSwitches
  // we use the number of switches between rendertargets to figure out where we are
  // in the pipeline. Yeah, it's flaky
  unsigned nrts = 0;
  // Count the number of times the 2 upper DoF rendertargets were set in doft[1]
  // & doft[2]
  std::array<unsigned, 3> doft;
  // HudDoneDetectionProgress
  // sequence: 5xHudHealthbar, 2-3xCategoryIconsSoulCount, followed by any other
  // texture signals end of normal Hud drawing
  // TODO: handle cursed
  unsigned hddp = 0;
  // main rendertarget for this frame
  IDirect3DSurface9Ptr mainRT;
  unsigned mainRTuses = 0;
  IDirect3DVertexDeclaration9Ptr prevVDecl;
  IDirect3DSurface9Ptr prevDepthStencilSurf;
  IDirect3DSurface9Ptr prevRenderTarget;
  IDirect3DStateBlock9Ptr prevStateBlock;
  bool haveOcclusionScale = false;
  float occlusionScale = 1;

  // Render state store/restore
  void storeRenderState();
  void restoreRenderState();
  unsigned isDof(unsigned width, unsigned height);
  void measureOcclusionScale();
  bool allowStateChanges() { return !onHudRT; }
  void finishHudRendering();
  void pauseHudRendering();
  void resumeHudRendering();
  void frameTimeManagement();
  ~RSManager() = default;

public:
  static RSManager& get() { return instance; }
  RSManager() = default;
  void setD3DDevice(IDirect3DDevice9* pD3Ddev) {
    SDLOG(LogLevel::Debug, "setD3DDevice: device: %p", pD3Ddev);
    d3ddev = pD3Ddev;
  }
  void initResources() noexcept;
  void releaseResources() noexcept;
  void setViewport(const D3DVIEWPORT9& vp) { viewport = vp; }
  bool isViewport(const RECT& r) {
    return (r.left == viewport.X) && (r.top == viewport.Y) && (r.bottom == viewport.Height) &&
           (r.right == viewport.Width);
  }
  D3DPRESENT_PARAMETERS
  adjustPresentationParameters(const D3DPRESENT_PARAMETERS* pPresentationParameters) noexcept;
  void takeHudlessScreenshot();
  void toggleSsao() { doSsao = !doSsao; }
  void toggleAA() { doAA = !doAA; }
  void toggleHUD() { hideHud = !hideHud; }
  void toggleHUDChanges() { doHud = !doHud; }
  void toggleDofGauss() { doDofGauss = !doDofGauss; }
  HRESULT redirectSetRenderTarget(DWORD RenderTargetIndex,
                                  IDirect3DSurface9* pRenderTarget) noexcept;
  HRESULT redirectSetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) noexcept;
  HRESULT redirectPresent(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride,
                          CONST RGNDATA* pDirtyRegion) noexcept;
  void togglePaused() { paused = !paused; }
  bool isPaused() { return paused; }
  HRESULT redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex,
                                         UINT NumVertices, UINT PrimitiveCount,
                                         CONST void* pIndexData, D3DFORMAT IndexDataFormat,
                                         CONST void* pVertexStreamZeroData,
                                         UINT VertexStreamZeroStride) noexcept;
  HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,
                                  CONST void* pVertexStreamZeroData,
                                  UINT VertexStreamZeroStride) noexcept;
  HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) noexcept;
  float getOcclusionScale() const { return occlusionScale; }
};
