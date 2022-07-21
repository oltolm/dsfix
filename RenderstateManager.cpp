#include "RenderstateManager.h"
#include "FPS.h"
#include "FXAA.h"
#include "GAUSS.h"
#include "SMAA.h"
#include "SSAO.h"
#include "Settings.h"
#include "WindowManager.h"
#include <spdlog/spdlog.h>
#ifndef _MSC_VER
#include <dxerr9.h>
#endif
#include <wrl.h>
using namespace Microsoft;

namespace {
unsigned int getDOFResolution() {
  unsigned int setting = Settings::get().getDOFOverrideResolution();
  return setting == 0 ? 360 : setting;
}
} // namespace

RSManager::RSManager(IDirect3DDevice9* pDevice) : m_pDevice(pDevice) { onReset(); }

void RSManager::setupAA() {
  unsigned int rw = Settings::get().getRenderWidth();
  unsigned int rh = Settings::get().getRenderHeight();
  if (Settings::get().getAAQuality()) {
    if (Settings::get().getAAType() == "SMAA") {
      smaa.reset(
          new SMAA(m_pDevice.Get(), rw, rh, (SMAA::Preset)(Settings::get().getAAQuality() - 1)));
      fxaa = nullptr;
    } else {
      fxaa.reset(
          new FXAA(m_pDevice.Get(), rw, rh, (FXAA::Quality)(Settings::get().getAAQuality() - 1)));
      smaa = nullptr;
    }
  } else {
    fxaa = nullptr;
    smaa = nullptr;
  }
}

void RSManager::setupSSAO() {
  SSAO::Type ssaoType;
  if (Settings::get().getSsaoType() == "VSSAO")
    ssaoType = SSAO::Type::VSSAO;
  else if (Settings::get().getSsaoType() == "VSSAO2")
    ssaoType = SSAO::Type::VSSAO2;
  else
    ssaoType = SSAO::Type::HBAO;
  if (Settings::get().getSsaoStrength()) {
    unsigned int rw = Settings::get().getRenderWidth();
    unsigned int rh = Settings::get().getRenderHeight();
    ssao.reset(new SSAO(m_pDevice.Get(), rw, rh, Settings::get().getSsaoStrength() - 1, ssaoType));
  } else {
    ssao = nullptr;
  }
}

void RSManager::setupDoF() {
  unsigned int dofRes = getDOFResolution();
  if (Settings::get().getDOFBlurAmount()) {
    gauss.reset(new GAUSS(m_pDevice.Get(), dofRes * 16 / 9, dofRes));
  } else {
    gauss = nullptr;
  }
}

void RSManager::onReset() {
  haveOcclusionScale = false;
  occlusionScale = 1;
  this->setupAA();
  this->setupSSAO();
  this->setupDoF();
  try {
    unsigned int rw = Settings::get().getRenderWidth();
    unsigned int rh = Settings::get().getRenderHeight();
    ThrowIfFailed(m_pDevice->CreateTexture(rw, rh, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                           D3DPOOL_DEFAULT, &rgbaBuffer1Tex, nullptr));
    ThrowIfFailed(rgbaBuffer1Tex->GetSurfaceLevel(0, &rgbaBuffer1Surf));
    ThrowIfFailed(m_pDevice->CreateDepthStencilSurface(rw, rh, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0,
                                                       FALSE, &depthStencilSurf, nullptr));
    ThrowIfFailed(m_pDevice->CreateStateBlock(D3DSBT_ALL, &prevStateBlock));
  } catch (const std::system_error& err) {
    spdlog::error(L"{}", DXGetErrorString9W(err.code().value()));
  }
}

HRESULT RSManager::redirectPresent(CONST RECT* pSourceRect, CONST RECT* pDestRect,
                                   HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) noexcept {
  nrts = 0;
  doft = {0};
  mainRT = nullptr;
  mainRTuses = 0;
  zSurf = nullptr;
  frameTimeManagement();
  try {
    return ThrowIfFailed(
        m_pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion));
  } catch (const std::system_error& err) {
    spdlog::error(L"Present: {}", DXGetErrorString9W(err.code().value()));
    if (err.code().value() == D3DERR_DEVICEREMOVED)
      removeFPSHook();
    return err.code().value();
  }
}

HRESULT RSManager::redirectSetRenderTarget(DWORD RenderTargetIndex,
                                           IDirect3DSurface9* pRenderTarget) noexcept {
  try {
    nrts++;
    if (nrts == 1) { /*we are switching to the RT that will be the main rendering
              target for this frame store it for later use*/
      mainRT = pRenderTarget;
      if (!haveOcclusionScale) {
        measureOcclusionScale();
      }
    }
    if (nrts == 11) { /*we are switching to the RT used to store the Z value in
              the 24 RGB bits (among other things) lets store it for later use*/
      zSurf = pRenderTarget;
    }
    if (mainRT && pRenderTarget == mainRT.Get()) {
      ++mainRTuses;
    }
    // we are switching away from the initial 3D-rendered image, do AA and SSAO
    if (mainRTuses == 2 && mainRT && zSurf && (ssao || smaa || fxaa)) {
      WRL::ComPtr<IDirect3DSurface9> oldRenderTarget;
      ThrowIfFailed(m_pDevice->GetRenderTarget(0, &oldRenderTarget));
      if (oldRenderTarget == mainRT) {
        WRL::ComPtr<IDirect3DTexture9> tex;
        ThrowIfFailed(oldRenderTarget->GetContainer(IID_PPV_ARGS(&tex)));
        // final renderbuffer has to be from texture, just making sure here
        if (tex) {
          // check size just to make even more sure
          D3DSURFACE_DESC desc;
          ThrowIfFailed(oldRenderTarget->GetDesc(&desc));
          if (desc.Width == Settings::get().getRenderWidth() &&
              desc.Height == Settings::get().getRenderHeight()) {
            WRL::ComPtr<IDirect3DTexture9> zTex;
            ThrowIfFailed(zSurf->GetContainer(IID_PPV_ARGS(&zTex)));
            storeRenderState();
            ThrowIfFailed(m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
            ThrowIfFailed(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
            ThrowIfFailed(m_pDevice->SetRenderState(
                D3DRS_COLORWRITEENABLE,
                D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE));
            // perform AA processing
            if (smaa || fxaa) {
              if (smaa)
                smaa->go(tex.Get(), tex.Get(), rgbaBuffer1Surf.Get(), SMAA::INPUT_COLOR);
              else
                fxaa->go(tex.Get(), rgbaBuffer1Surf.Get());
              ThrowIfFailed(m_pDevice->StretchRect(rgbaBuffer1Surf.Get(), nullptr,
                                                   oldRenderTarget.Get(), nullptr, D3DTEXF_NONE));
            }
            // perform SSAO
            if (ssao) {
              ssao->go(tex.Get(), zTex.Get(), rgbaBuffer1Surf.Get());
              ThrowIfFailed(m_pDevice->StretchRect(rgbaBuffer1Surf.Get(), nullptr,
                                                   oldRenderTarget.Get(), nullptr, D3DTEXF_NONE));
            }
            restoreRenderState();
          }
        }
      }
    }
    // DoF blur stuff
    if (gauss) {
      WRL::ComPtr<IDirect3DSurface9> oldRenderTarget;
      ThrowIfFailed(m_pDevice->GetRenderTarget(0, &oldRenderTarget));
      D3DSURFACE_DESC desc;
      ThrowIfFailed(oldRenderTarget->GetDesc(&desc));
      unsigned int dofIndex = isDof(desc.Width, desc.Height);
      if (dofIndex) {
        doft[dofIndex]++;
        if (dofIndex == 1 && doft[1] == 4) {
          WRL::ComPtr<IDirect3DTexture9> oldRTtex;
          ThrowIfFailed(oldRenderTarget->GetContainer(IID_PPV_ARGS(&oldRTtex)));
          if (oldRTtex) {
            storeRenderState();
            for (size_t i = 0; i < Settings::get().getDOFBlurAmount(); ++i)
              gauss->go(oldRTtex.Get(), oldRenderTarget.Get());
            restoreRenderState();
          }
        }
      }
    }
    return ThrowIfFailed(m_pDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget));
  } catch (const std::system_error& err) {
    spdlog::error(L"{}", DXGetErrorString9W(err.code().value()));
    return err.code().value();
  }
}

// Measure the occlusion query result of drawing a square of a known size.
// The result is affected by the rendering resolution which is
// known, but might also be affected by driver-enforced antialiasing.
// This result is used to scale the result of further occlusion
// queries into the expected range of values.
void RSManager::measureOcclusionScale() {
  static const D3DVERTEXELEMENT9 vertexElements[2] = {
      {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, D3DDECL_END()};
  WRL::ComPtr<IDirect3DVertexDeclaration9> vertexDeclaration;
  WRL::ComPtr<ID3DXBuffer> errorBuffer;
  static const char vertexShaderSource[] =
      R"(vs_3_0
 dcl_position v0
 dcl_position o0
 mov o0, v0)";
  WRL::ComPtr<ID3DXBuffer> vertexShaderBuffer;
  WRL::ComPtr<IDirect3DVertexShader9> vertexShader;
  static const char pixelShaderSource[] = R"(ps_3_0
 def c0, 0, 0, 0, 0
 mov_pp oC0, c0.x)";
  WRL::ComPtr<ID3DXBuffer> pixelShaderBuffer;
  WRL::ComPtr<IDirect3DPixelShader9> pixelShader;
  WRL::ComPtr<IDirect3DQuery9> query;
  DWORD pixelsVisible = 0;
  HRESULT hr;
  haveOcclusionScale = true;
  float width = 24.0 / 1024;
  float height = 24.0 / 720;
  const float vertexData[4][3] = {
      {-width, -height, 0.5},
      {width, -height, 0.5},
      {width, height, 0.5},
      {-width, height, 0.5},
  };
  ThrowIfFailed(m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1, 0));
  ThrowIfFailed(m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
  ThrowIfFailed(m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL));
  ThrowIfFailed(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
  ThrowIfFailed(m_pDevice->CreateVertexDeclaration(vertexElements, &vertexDeclaration));
  ThrowIfFailed(m_pDevice->SetVertexDeclaration(vertexDeclaration.Get()));
  ThrowIfFailed(::D3DXAssembleShader(vertexShaderSource, sizeof(vertexShaderSource), nullptr,
                                     nullptr, 0, &vertexShaderBuffer, &errorBuffer));
  ThrowIfFailed(m_pDevice->CreateVertexShader(
      static_cast<DWORD*>(vertexShaderBuffer->GetBufferPointer()), &vertexShader));
  ThrowIfFailed(m_pDevice->SetVertexShader(vertexShader.Get()));
  ThrowIfFailed(::D3DXAssembleShader(pixelShaderSource, sizeof(pixelShaderSource), nullptr, nullptr,
                                     0, &pixelShaderBuffer, &errorBuffer));
  ThrowIfFailed(m_pDevice->CreatePixelShader(
      static_cast<DWORD*>(pixelShaderBuffer->GetBufferPointer()), &pixelShader));
  ThrowIfFailed(m_pDevice->SetPixelShader(pixelShader.Get()));
  ThrowIfFailed(m_pDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query));
  ThrowIfFailed(m_pDevice->BeginScene());
  ThrowIfFailed(query->Issue(D3DISSUE_BEGIN));
  ThrowIfFailed(
      m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertexData, sizeof(vertexData[0])));
  ThrowIfFailed(query->Issue(D3DISSUE_END));
  while ((hr = query->GetData(&pixelsVisible, sizeof(pixelsVisible), D3DGETDATA_FLUSH)) == S_FALSE)
    ;
  ThrowIfFailed(hr);
  occlusionScale = pixelsVisible == 0 ? 1 : pixelsVisible / 576.0;
  ThrowIfFailed(m_pDevice->EndScene());
}

unsigned int RSManager::isDof(unsigned width, unsigned height) {
  unsigned int topWidth = getDOFResolution() * 16 / 9;
  unsigned int topHeight = getDOFResolution();
  if (width == topWidth && height == topHeight)
    return 1;
  if (width == topWidth / 2 && height == topHeight / 2)
    return 2;
  return 0;
}

void RSManager::storeRenderState() {
  ThrowIfFailed(prevStateBlock->Capture());
  ThrowIfFailed(m_pDevice->GetVertexDeclaration(&prevVDecl));
  auto hr = m_pDevice->GetDepthStencilSurface(&prevDepthStencilSurf);
  if (hr != D3DERR_NOTFOUND)
    ThrowIfFailed(hr);
  ThrowIfFailed(m_pDevice->SetDepthStencilSurface(depthStencilSurf.Get()));
}

void RSManager::restoreRenderState() {
  if (prevVDecl)
    ThrowIfFailed(m_pDevice->SetVertexDeclaration(prevVDecl.Get()));
  ThrowIfFailed(
      m_pDevice->SetDepthStencilSurface(prevDepthStencilSurf.Get())); // also restore nullptr!
  ThrowIfFailed(prevStateBlock->Apply());
}

void RSManager::frameTimeManagement() {
  double renderTime = getElapsedTime() - lastPresentTime;
  // implement FPS cap
  if (Settings::get().getUnlockFPS()) {
    double desiredRenderTime = (1000.0 / Settings::get().getFPSLimit()) - 0.1;
    while (renderTime < desiredRenderTime) {
      ::SwitchToThread();
      renderTime = getElapsedTime() - lastPresentTime;
    }
  }
  lastPresentTime = getElapsedTime();
}
