#include "RenderstateManager.h"
#include "Detouring.h"
#include "FPS.h"
#include "Hash.h"
#include "Settings.h"
#include "TextureManager.h"
#include "WindowManager.h"
#include "log.h"
#include "main.h"
#include "tinyformat.h"
#include "util.h"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>

RSManager RSManager::instance;
namespace {
unsigned getDOFResolution() {
  unsigned setting = Settings::get().getDOFOverrideResolution();
  return setting == 0 ? 360 : setting;
}
} // namespace

void RSManager::initResources() noexcept {
  unsigned rw = Settings::get().getRenderWidth(), rh = Settings::get().getRenderHeight();
  haveOcclusionScale = false;
  occlusionScale = 1;
  unsigned dofRes = getDOFResolution();
  if (Settings::get().getAAQuality()) {
    if (Settings::get().getAAType() == "SMAA") {
      smaa.reset(new SMAA(d3ddev, rw, rh, (SMAA::Preset)(Settings::get().getAAQuality() - 1)));
    } else {
      fxaa.reset(new FXAA(d3ddev, rw, rh, (FXAA::Quality)(Settings::get().getAAQuality() - 1)));
    }
  }
  SSAO::Type ssaoType = Settings::get().getSsaoType() == "VSSAO" ? SSAO::VSSAO : SSAO::VSSAO2;

  if (Settings::get().getSsaoStrength())
    ssao.reset(new SSAO(d3ddev, rw, rh, Settings::get().getSsaoStrength() - 1, ssaoType));
  if (Settings::get().getDOFBlurAmount())
    gauss.reset(new GAUSS(d3ddev, dofRes * 16 / 9, dofRes));
  if (Settings::get().getEnableHudMod())
    hud.reset(new HUD(d3ddev, rw, rh));
  try {
    throw_if_fail(d3ddev->CreateTexture(rw, rh, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &rgbaBuffer1Tex, NULL));
    throw_if_fail(rgbaBuffer1Tex->GetSurfaceLevel(0, &rgbaBuffer1Surf));
    throw_if_fail(d3ddev->CreateDepthStencilSurface(rw, rh, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0,
                                                    FALSE, &depthStencilSurf, NULL));
    throw_if_fail(d3ddev->CreateStateBlock(D3DSBT_ALL, &prevStateBlock));
    if (!initialized) { // on first init only
      init();
      startDetour();
      initialized = true;
    }
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}

void RSManager::releaseResources() noexcept {
  rgbaBuffer1Surf = nullptr;
  rgbaBuffer1Tex = nullptr;
  depthStencilSurf = nullptr;
  prevStateBlock = nullptr;
  smaa = nullptr;
  fxaa = nullptr;
  ssao = nullptr;
  gauss = nullptr;
  hud = nullptr;
}

HRESULT RSManager::redirectPresent(CONST RECT* pSourceRect, CONST RECT* pDestRect,
                                   HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) noexcept {
  if (timingIntroMode) {
    skippedPresents++;
    if (skippedPresents >= 1200u && !Settings::get().getUnlockFPS()) {
      SDLOG(LogLevel::Info, "Intro mode ended (timeout)!");
      timingIntroMode = false;
    }
    if (skippedPresents >= 3000u) {
      SDLOG(LogLevel::Info, "Intro mode ended (full timeout)!");
      timingIntroMode = false;
    }
    return S_OK;
  }
  skippedPresents = 0;
  hudStarted = false;
  nrts = 0;
  doft = {0};
  mainRT = nullptr;
  mainRTuses = 0;
  zSurf = nullptr;
  frameTimeManagement();
  try {
    return throw_if_fail(
        d3ddev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion));
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "redirectPresent: %s, error code: 0x%x", err.what(), err.code().value());
    if (err.code().value() == D3DERR_DEVICEREMOVED)
      removeFPSHook();
    return err.code().value();
  }
}

D3DPRESENT_PARAMETERS RSManager::adjustPresentationParameters(
    const D3DPRESENT_PARAMETERS* pPresentationParameters) noexcept {
  D3DPRESENT_PARAMETERS ret = *pPresentationParameters;
  if (Settings::get().getForceWindowed()) {
    WindowManager::get().resize(Settings::get().getPresentWidth(),
                                Settings::get().getPresentHeight());
    ret.Windowed = TRUE;
    ret.FullScreen_RefreshRateInHz = 0;
  } else if (Settings::get().getForceFullscreen()) {
    ret.Windowed = FALSE;
    ret.FullScreen_RefreshRateInHz = Settings::get().getFullscreenHz();
  }
  if (Settings::get().getForceFullscreen() || Settings::get().getForceWindowed()) {
    ret.PresentationInterval = Settings::get().getEnableVsync() ? D3DPRESENT_INTERVAL_DEFAULT
                                                                : D3DPRESENT_INTERVAL_IMMEDIATE;
    ret.BackBufferWidth = Settings::get().getPresentWidth();
    ret.BackBufferHeight = Settings::get().getPresentHeight();
  }
  return ret;
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
    if (mainRT && pRenderTarget == mainRT) {
      ++mainRTuses;
    }
    // we are switching away from the initial 3D-rendered image, do AA and SSAO
    if (mainRTuses == 2 && mainRT && zSurf && ((ssao && doSsao) || (doAA && (smaa || fxaa)))) {
      IDirect3DSurface9Ptr oldRenderTarget;
      throw_if_fail(d3ddev->GetRenderTarget(0, &oldRenderTarget));
      if (oldRenderTarget == mainRT) {
        IDirect3DTexture9Ptr tex;
        throw_if_fail(oldRenderTarget->GetContainer(IID_IDirect3DTexture9, (void**)&tex));
        // final renderbuffer has to be from texture, just making sure here
        if (tex) {
          // check size just to make even more sure
          D3DSURFACE_DESC desc;
          throw_if_fail(oldRenderTarget->GetDesc(&desc));
          if (desc.Width == Settings::get().getRenderWidth() &&
              desc.Height == Settings::get().getRenderHeight()) {
            IDirect3DTexture9Ptr zTex;
            throw_if_fail(zSurf->GetContainer(IID_IDirect3DTexture9, (void**)&zTex));
            storeRenderState();
            throw_if_fail(d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
            throw_if_fail(d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
            throw_if_fail(d3ddev->SetRenderState(
                D3DRS_COLORWRITEENABLE,
                D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE));
            // perform AA processing
            if (doAA && (smaa || fxaa)) {
              if (smaa)
                smaa->go(tex, tex, rgbaBuffer1Surf, SMAA::INPUT_COLOR);
              else
                fxaa->go(tex, rgbaBuffer1Surf);
              throw_if_fail(
                  d3ddev->StretchRect(rgbaBuffer1Surf, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
            }
            // perform SSAO
            if (ssao && doSsao) {
              ssao->go(tex, zTex, rgbaBuffer1Surf);
              throw_if_fail(
                  d3ddev->StretchRect(rgbaBuffer1Surf, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
            }
            restoreRenderState();
          }
        }
      }
    }
    // DoF blur stuff
    if (gauss && doDofGauss) {
      IDirect3DSurface9Ptr oldRenderTarget;
      throw_if_fail(d3ddev->GetRenderTarget(0, &oldRenderTarget));
      D3DSURFACE_DESC desc;
      throw_if_fail(oldRenderTarget->GetDesc(&desc));
      unsigned dofIndex = isDof(desc.Width, desc.Height);
      if (dofIndex) {
        doft[dofIndex]++;
        if (dofIndex == 1 && doft[1] == 4) {
          IDirect3DTexture9Ptr oldRTtex;
          throw_if_fail(oldRenderTarget->GetContainer(IID_IDirect3DTexture9, (void**)&oldRTtex));
          if (oldRTtex) {
            storeRenderState();
            for (size_t i = 0; i < Settings::get().getDOFBlurAmount(); ++i)
              gauss->go(oldRTtex, oldRenderTarget);
            restoreRenderState();
          }
        }
      }
    }
    // Timing for hudless screenshots
    if (mainRTuses == 11 && takeScreenshot) {
      IDirect3DSurface9Ptr oldRenderTarget;
      throw_if_fail(d3ddev->GetRenderTarget(0, &oldRenderTarget));
      if (oldRenderTarget != mainRT) {
        static bool toggleSS = false;
        toggleSS = !toggleSS;
        if (!toggleSS) {
          takeScreenshot = false;
          std::wstringstream filename;
          std::time_t time = std::time(nullptr);
          filename << std::put_time(std::localtime(&time), L"screenshot_%Y-%m-%d_%H-%M-%S.jpg");
          std::wstring destfile = Settings::get().getScreenshotDir() + L"\\" + filename.str();
          SDLOG(LogLevel::Info, "Capturing screenshot - to %s", destfile);
          D3DSURFACE_DESC desc;
          throw_if_fail(oldRenderTarget->GetDesc(&desc));
          IDirect3DSurface9Ptr convertedSurface;
          throw_if_fail(d3ddev->CreateRenderTarget(desc.Width, desc.Height, D3DFMT_X8R8G8B8,
                                                   D3DMULTISAMPLE_NONE, 0, true, &convertedSurface,
                                                   NULL));
          throw_if_fail(::D3DXLoadSurfaceFromSurface(convertedSurface, NULL, NULL, oldRenderTarget,
                                                     NULL, NULL, D3DX_FILTER_POINT, 0));
          throw_if_fail(::D3DXSaveSurfaceToFileW(destfile.c_str(), D3DXIFF_JPG, convertedSurface,
                                                 NULL, NULL));
        }
      }
    }
    if (rddp >= 4) { // we just finished rendering the frame (pre-HUD)
      IDirect3DSurface9Ptr oldRenderTarget;
      throw_if_fail(d3ddev->GetRenderTarget(0, &oldRenderTarget));
      IDirect3DTexture9Ptr tex;
      throw_if_fail(oldRenderTarget->GetContainer(IID_IDirect3DTexture9, (void**)&tex));
      // final renderbuffer has to be from texture, just making sure here
      if (tex) {
        // check size just to make even more sure
        D3DSURFACE_DESC desc;
        throw_if_fail(oldRenderTarget->GetDesc(&desc));
        if (desc.Width == Settings::get().getRenderWidth() &&
            desc.Height == Settings::get().getRenderHeight()) {
          // HUD stuff
          if (hud && doHud && rddp == 9) {
            hddp = 0;
            onHudRT = true;
            throw_if_fail(d3ddev->SetRenderTarget(0, rgbaBuffer1Surf));
            throw_if_fail(
                d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 0.0f, 0));
            prevRenderTarget = pRenderTarget;
            throw_if_fail(d3ddev->SetRenderState(
                D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
                                            D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA));
            throw_if_fail(d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD));
            throw_if_fail(d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
            throw_if_fail(d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT));
            return S_OK;
          }
        }
      }
    }
    if (onHudRT) {
      finishHudRendering();
    }
    if (rddp < 4 || rddp > 8)
      rddp = 0;
    else
      rddp++;
    return throw_if_fail(d3ddev->SetRenderTarget(RenderTargetIndex, pRenderTarget));
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "redirectSetRenderTarget: %s", err.what());
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
  IDirect3DVertexDeclaration9Ptr vertexDeclaration;
  ID3DXBufferPtr errorBuffer;
  static const char vertexShaderSource[] =
      R"(vs_3_0
 dcl_position v0
 dcl_position o0
 mov o0, v0)";
  ID3DXBufferPtr vertexShaderBuffer;
  IDirect3DVertexShader9Ptr vertexShader;
  static const char pixelShaderSource[] = R"(ps_3_0
 def c0, 0, 0, 0, 0
 mov_pp oC0, c0.x)";
  ID3DXBufferPtr pixelShaderBuffer;
  IDirect3DPixelShader9Ptr pixelShader;
  IDirect3DQuery9Ptr query;
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
  throw_if_fail(d3ddev->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1, 0));
  throw_if_fail(d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
  throw_if_fail(d3ddev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL));
  throw_if_fail(d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
  throw_if_fail(d3ddev->CreateVertexDeclaration(vertexElements, &vertexDeclaration));
  throw_if_fail(d3ddev->SetVertexDeclaration(vertexDeclaration));
  throw_if_fail(::D3DXAssembleShader(vertexShaderSource, sizeof(vertexShaderSource), nullptr,
                                     nullptr, 0, &vertexShaderBuffer, &errorBuffer));
  throw_if_fail(d3ddev->CreateVertexShader(
      static_cast<DWORD*>(vertexShaderBuffer->GetBufferPointer()), &vertexShader));
  throw_if_fail(d3ddev->SetVertexShader(vertexShader));
  throw_if_fail(::D3DXAssembleShader(pixelShaderSource, sizeof(pixelShaderSource), nullptr, nullptr,
                                     0, &pixelShaderBuffer, &errorBuffer));
  throw_if_fail(d3ddev->CreatePixelShader(
      static_cast<DWORD*>(pixelShaderBuffer->GetBufferPointer()), &pixelShader));
  throw_if_fail(d3ddev->SetPixelShader(pixelShader));
  throw_if_fail(d3ddev->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query));
  throw_if_fail(d3ddev->BeginScene());
  throw_if_fail(query->Issue(D3DISSUE_BEGIN));
  throw_if_fail(d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertexData, sizeof(vertexData[0])));
  throw_if_fail(query->Issue(D3DISSUE_END));
  while ((hr = query->GetData(&pixelsVisible, sizeof(pixelsVisible), D3DGETDATA_FLUSH)) == S_FALSE)
    ;
  throw_if_fail(hr);
  occlusionScale = pixelsVisible == 0 ? 1 : pixelsVisible / 576.0;
  throw_if_fail(d3ddev->EndScene());
}

HRESULT RSManager::redirectSetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) noexcept {
  auto& tm = TextureManager::get();
  try {
    if (pTexture == NULL)
      return throw_if_fail(d3ddev->SetTexture(Stage, pTexture));
    if (Settings::get().getSkipIntro() && !timingIntroMode &&
        tm.isTextureBandainamcoLogo(pTexture)) {
      SDLOG(LogLevel::Info, "Intro mode started!");
      timingIntroMode = true;
    }
    if (timingIntroMode && (tm.isTextureGuiElements1(pTexture) ||
                            tm.isTextureMenuscreenLogo(pTexture) || tm.isTextureText(pTexture))) {
      SDLOG(LogLevel::Info, "Intro mode ended due to texture!");
      timingIntroMode = false;
    }
    if (!hudStarted && tm.isTextureHudHealthbar(pTexture)) {
      hudStarted = true;
    }
    if ((rddp == 0 && Stage == 0) || (rddp == 1 && Stage == 1) || (rddp == 2 && Stage == 2) ||
        (rddp == 3 && Stage == 3)) {
      ++rddp;
    } else {
      rddp = 0;
    }
    return throw_if_fail(d3ddev->SetTexture(Stage, pTexture));
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
    return err.code().value();
  }
}

void RSManager::takeHudlessScreenshot() {
  takeScreenshot = true;
  SDLOG(LogLevel::Info, "takeScreenshot: %s", takeScreenshot ? "true" : "false");
}

HRESULT RSManager::redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex,
                                                  UINT NumVertices, UINT PrimitiveCount,
                                                  CONST void* pIndexData, D3DFORMAT IndexDataFormat,
                                                  CONST void* pVertexStreamZeroData,
                                                  UINT VertexStreamZeroStride) noexcept {
  auto& tm = TextureManager::get();
  if (hudStarted && hideHud) {
    return D3D_OK;
  }
  try {
    bool isTargetIndicator = false;
    if (pausedHudRT) {
      IDirect3DBaseTexture9Ptr t;
      throw_if_fail(d3ddev->GetTexture(0, &t));
      // check for target indicator
      if (tm.isTextureHudHealthbar(t)) {
        const INT16* vertices = static_cast<const INT16*>(pVertexStreamZeroData);
        if (vertices[3] > -2000) {
          resumeHudRendering();
        }
      } else {
        resumeHudRendering();
      }
    }
    if (onHudRT) {
      IDirect3DBaseTexture9Ptr t;
      throw_if_fail(d3ddev->GetTexture(0, &t));
      if ((hddp < 5 && tm.isTextureHudHealthbar(t)) ||
          (hddp >= 5 && hddp < 7 && tm.isTextureCategoryIconsHumanityCount(t)) ||
          (hddp >= 7 && !tm.isTextureCategoryIconsHumanityCount(t))) {
        hddp++;
      }
      // check for target indicator
      if (tm.isTextureHudHealthbar(t)) {
        const INT16* vertices = static_cast<const INT16*>(pVertexStreamZeroData);
        if (vertices[3] < -2000) {
          isTargetIndicator = true;
          pauseHudRendering();
        }
      }
      if (hddp == 8) {
        finishHudRendering();
      } else if (!isTargetIndicator) {
        // d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE,
        //                        D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
        //                            D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
        // d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
        // d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        // d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
      }
    }
    return throw_if_fail(d3ddev->DrawIndexedPrimitiveUP(
        PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat,
        pVertexStreamZeroData, VertexStreamZeroStride));
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
    return err.code().value();
  }
}

HRESULT RSManager::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,
                                           CONST void* pVertexStreamZeroData,
                                           UINT VertexStreamZeroStride) noexcept {
  auto& tm = TextureManager::get();
  try {
    if (hudStarted && hideHud) {
      IDirect3DBaseTexture9Ptr t;
      throw_if_fail(d3ddev->GetTexture(0, &t));
      bool hide =
          tm.isTextureText(t) || tm.isTextureButtonsEffects(t) || tm.isTextureHudEffectIcons(t);
      if (hide)
        return D3D_OK;
    }
    if (pausedHudRT) {
      IDirect3DBaseTexture9Ptr t;
      throw_if_fail(d3ddev->GetTexture(0, &t));
      bool isText = tm.isTextureText(t);
      if (isText && PrimitiveCount >= 12)
        resumeHudRendering();
    }
    bool subbed = false;
    if (onHudRT) {
      IDirect3DBaseTexture9Ptr t;
      throw_if_fail(d3ddev->GetTexture(0, &t));
      bool isText = tm.isTextureText(t);
      bool isSub = tm.isTextureText00(t);
      if (isSub) {
        pauseHudRendering();
        subbed = true;
      }
    }
    HRESULT hr = throw_if_fail(d3ddev->DrawPrimitiveUP(
        PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride));
    if (subbed)
      resumeHudRendering();
    return hr;
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
    return err.code().value();
  }
}

unsigned RSManager::isDof(unsigned width, unsigned height) {
  unsigned topWidth = getDOFResolution() * 16 / 9, topHeight = getDOFResolution();
  if (width == topWidth && height == topHeight)
    return 1;
  if (width == topWidth / 2 && height == topHeight / 2)
    return 2;
  return 0;
}

void RSManager::storeRenderState() {
  throw_if_fail(prevStateBlock->Capture());
  throw_if_fail(d3ddev->GetVertexDeclaration(&prevVDecl));
  auto hr = d3ddev->GetDepthStencilSurface(&prevDepthStencilSurf);
  if (hr != D3DERR_NOTFOUND)
    throw_if_fail(hr);
  throw_if_fail(d3ddev->SetDepthStencilSurface(depthStencilSurf));
}

void RSManager::restoreRenderState() {
  if (prevVDecl) {
    throw_if_fail(d3ddev->SetVertexDeclaration(prevVDecl));
  }
  throw_if_fail(d3ddev->SetDepthStencilSurface(prevDepthStencilSurf)); // also restore NULL!
  throw_if_fail(prevStateBlock->Apply());
}

void RSManager::finishHudRendering() {
  throw_if_fail(d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED |
                                                                   D3DCOLORWRITEENABLE_GREEN |
                                                                   D3DCOLORWRITEENABLE_BLUE));
  throw_if_fail(d3ddev->SetRenderTarget(0, prevRenderTarget));
  onHudRT = false;
  // draw HUD to screen
  storeRenderState();
  hud->go(rgbaBuffer1Tex, prevRenderTarget);
  restoreRenderState();
}

void RSManager::pauseHudRendering() {
  throw_if_fail(d3ddev->SetRenderTarget(0, prevRenderTarget));
  throw_if_fail(d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED |
                                                                   D3DCOLORWRITEENABLE_GREEN |
                                                                   D3DCOLORWRITEENABLE_BLUE));
  throw_if_fail(d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA));
  onHudRT = false;
  pausedHudRT = true;
}

void RSManager::resumeHudRendering() {
  throw_if_fail(d3ddev->SetRenderTarget(0, rgbaBuffer1Surf));
  throw_if_fail(d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE,
                                       D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
                                           D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA));
  throw_if_fail(d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1));
  onHudRT = true;
  pausedHudRT = false;
}

HRESULT RSManager::redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) noexcept {
  try {
    if (State == D3DRS_COLORWRITEENABLE && !allowStateChanges())
      return D3D_OK;
    return throw_if_fail(d3ddev->SetRenderState(State, Value));
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
    return err.code().value();
  }
}

void RSManager::frameTimeManagement() {
  double renderTime = getElapsedTime() - lastPresentTime;
  // implement FPS cap
  if (Settings::get().getUnlockFPS()) {
    double desiredRenderTime = (1000.0 / Settings::get().getCurrentFPSLimit()) - 0.1;
    while (renderTime < desiredRenderTime) {
      ::SwitchToThread();
      renderTime = getElapsedTime() - lastPresentTime;
    }
  }
  lastPresentTime = getElapsedTime();
}
