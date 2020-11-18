#pragma once
#include "FXAA.h"
#include "GAUSS.h"
#include "Hud.h"
#include "SMAA.h"
#include "SSAO.h"
#include "TextureManager.h"
#include <array>
#include <memory>
#include <spdlog/spdlog.h>
#include <wrl.h>

class RSManager {
private:
  D3DVIEWPORT9 viewport;
  Microsoft::WRL::ComPtr<IDirect3DDevice9> d3ddev;
  double lastPresentTime = 0;
  bool doAA = false;
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
  std::array<unsigned int, 3> doft;
  // main rendertarget for this frame
  Microsoft::WRL::ComPtr<IDirect3DSurface9> mainRT;
  unsigned int mainRTuses = 0;
  Microsoft::WRL::ComPtr<IDirect3DVertexDeclaration9> prevVDecl;
  Microsoft::WRL::ComPtr<IDirect3DSurface9> prevDepthStencilSurf;
  Microsoft::WRL::ComPtr<IDirect3DStateBlock9> prevStateBlock;
  bool haveOcclusionScale = false;
  float occlusionScale = 1;

  // HUD begin
  bool doHud = true;
  std::unique_ptr<HUD> hud;
  bool hideHud = false;
  bool onHudRT = false;
  bool pausedHudRT = false;
  bool hudStarted = false;
  // HudDoneDetectionProgress
  // sequence: 5xHudHealthbar, 2-3xCategoryIconsSoulCount, followed by any other
  // texture signals end of normal Hud drawing
  // TODO: handle cursed
  unsigned int hddp = 0;

  Microsoft::WRL::ComPtr<IDirect3DSurface9> prevRenderTarget;

  bool allowStateChanges() { return !onHudRT; }
  void finishHudRendering();
  void resumeHudRendering();
  void pauseHudRendering();
  // HUD end

  unsigned int isDof(unsigned int width, unsigned int height);
  void measureOcclusionScale();
  void frameTimeManagement();

public:
  ~RSManager();
  RSManager(IDirect3DDevice9* pD3Ddev);

  void setupAA();
  void setupSSAO();
  void setupDoF();
  void setupHUD();

  void onReset(D3DPRESENT_PARAMETERS* pPresentationParameters);
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
  HRESULT redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex,
                                         UINT NumVertices, UINT PrimitiveCount,
                                         CONST void* pIndexData, D3DFORMAT IndexDataFormat,
                                         CONST void* pVertexStreamZeroData,
                                         UINT VertexStreamZeroStride) noexcept;
  HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,
                                  CONST void* pVertexStreamZeroData,
                                  UINT VertexStreamZeroStride) noexcept;
  float getOcclusionScale() const { return occlusionScale; }
  // Render state store/restore
  void storeRenderState();
  void restoreRenderState();
  // HUD begin
  HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
  // HUD end
};
