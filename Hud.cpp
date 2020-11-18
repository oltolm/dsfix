#include "Hud.h"
#include "Settings.h"
#include "log.h"
#include "util.h"
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

HUD::HUD(IDirect3DDevice9* device, int width, int height)
    : Effect(device), width(width), height(height) {
  // Load effect from file
  SDLOG(LogLevel::Info, "Hud Effect load");
  ID3DXBufferPtr errors;
  fs::path srcfile = GetModuleDirectoryPath() / L"dsfix\\HUD.fx";
  HRESULT hr = ::D3DXCreateEffectFromFileW(device, srcfile.c_str(), NULL, NULL,
                                           D3DXFX_NOT_CLONEABLE, NULL, &effect, &errors);
  if (FAILED(D3D_OK)) {
    SDLOG(LogLevel::Error, "ERRORS:");
    SDLOG(LogLevel::Error, " %s", errors->GetBufferPointer());
  }
  // get handles
  frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
  opacityHandle = effect->GetParameterByName(NULL, "opacity");
}

void HUD::go(IDirect3DTexture9* input, IDirect3DSurface9* dst) {
  throw_if_fail(device->SetVertexDeclaration(vertexDeclaration));
  throw_if_fail(device->SetRenderTarget(0, dst));
  throw_if_fail(effect->SetTexture(frameTexHandle, input));
  float scale = Settings::get().getHudScaleFactor();
  float iscale = 1.0f - scale;
  UINT passes;
  // upper left
  throw_if_fail(effect->SetFloat(opacityHandle, Settings::get().getHudTopLeftOpacity()));
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(0));
  rect(0.0f, 0.0f, 1.0f, 0.21f, 0.0f, 0.0f, 1.0f * scale, 0.21f * scale);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
  // lower left
  throw_if_fail(effect->SetFloat(opacityHandle, Settings::get().getHudBottomLeftOpacity()));
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(0));
  if (Settings::get().getEnableMinimalHud()) {
    rect(0.145f, 0.527f, 0.074f, 0.204f, 0.1f * scale, 0.77f + 0.2f * iscale, 0.074f * scale,
         0.204f * scale);
    rect(0.145f, 0.731f, 0.074f, 0.204f, 0.1f * scale + 0.074f * scale + 0.01f,
         0.77f + 0.2f * iscale, 0.074f * scale, 0.204f * scale);
  } else {
    rect(0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.5f + 0.5f * iscale, 0.5f * scale, 0.5f * scale);
  }
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
  // lower right
  throw_if_fail(effect->SetFloat(opacityHandle, Settings::get().getHudBottomRightOpacity()));
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(0));
  rect(0.8f, 0.8f, 0.2f, 0.2f, 0.8f + 0.2f * iscale, 0.8f + 0.2f * iscale, 0.2f * scale,
       0.2f * scale);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
  // center
  throw_if_fail(effect->SetFloat(opacityHandle, 1.0f));
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(0));
  rect(0.37f, 0.22f, 0.4f, 0.5f, 0.37f + 0.15f * iscale, 0.22f + 0.15f * iscale, 0.4f * scale,
       0.5f * scale);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
}

void HUD::rect(float srcLeft, float srcTop, float srcWidth, float srcHeight, float trgLeft,
               float trgTop, float trgWidth, float trgHeight) {
  trgTop = -(trgTop * 2.0f - 1.0f);
  trgLeft = trgLeft * 2.0f - 1.0f;
  float trgRight = trgLeft + trgWidth * 2.0f;
  float trgBottom = trgTop - trgHeight * 2.0f;
  float srcRight = srcLeft + srcWidth;
  float srcBottom = srcTop + srcHeight;
  float quad[4][5] = {{trgLeft, trgTop, 0.5f, srcLeft, srcTop},
                      {trgRight, trgTop, 0.5f, srcRight, srcTop},
                      {trgLeft, trgBottom, 0.5f, srcLeft, srcBottom},
                      {trgRight, trgBottom, 0.5f, srcRight, srcBottom}};
  throw_if_fail(device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])));
}
