#include "Hud.h"
#include "Settings.h"
#include "util.h"
#include <filesystem>
#include <wrl.h>

using namespace Microsoft;

namespace fs = std::filesystem;

HUD::HUD(IDirect3DDevice9* device, int width, int height)
    : Effect(device), width(width), height(height) {
  // Load effect from file
  spdlog::info("Hud Effect load");
  WRL::ComPtr<ID3DXBuffer> errors;
  fs::path srcfile = GetModuleDirectoryPath() / L"dsfix\\HUD.fx";
  HRESULT hr = ::D3DXCreateEffectFromFileW(device, srcfile.c_str(), nullptr, nullptr,
                                           D3DXFX_NOT_CLONEABLE, nullptr, &effect, &errors);
  if (FAILED(hr)) {
    spdlog::error("ERRORS:");
    spdlog::error(" {}", errors->GetBufferPointer());
  }
  // get handles
  frameTexHandle = effect->GetParameterByName(nullptr, "frameTex2D");
  opacityHandle = effect->GetParameterByName(nullptr, "opacity");
}

void HUD::go(IDirect3DTexture9* input, IDirect3DSurface9* dst) {
  ThrowIfFailed(device->SetVertexDeclaration(vertexDeclaration.Get()));
  ThrowIfFailed(device->SetRenderTarget(0, dst));
  ThrowIfFailed(effect->SetTexture(frameTexHandle, input));
  float scale = Settings::get().getHudScaleFactor();
  float iscale = 1.0f - scale;
  UINT passes;
  // upper left
  ThrowIfFailed(effect->SetFloat(opacityHandle, Settings::get().getHudTopLeftOpacity()));
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(0));
  rect(0.0f, 0.0f, 1.0f, 0.21f, 0.0f, 0.0f, 1.0f * scale, 0.21f * scale);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
  // lower left
  ThrowIfFailed(effect->SetFloat(opacityHandle, Settings::get().getHudBottomLeftOpacity()));
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(0));
  if (Settings::get().getEnableMinimalHud()) {
    rect(0.145f, 0.527f, 0.074f, 0.204f, 0.1f * scale, 0.77f + 0.2f * iscale, 0.074f * scale,
         0.204f * scale);
    rect(0.145f, 0.731f, 0.074f, 0.204f, 0.1f * scale + 0.074f * scale + 0.01f,
         0.77f + 0.2f * iscale, 0.074f * scale, 0.204f * scale);
  } else {
    rect(0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.5f + 0.5f * iscale, 0.5f * scale, 0.5f * scale);
  }
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
  // lower right
  ThrowIfFailed(effect->SetFloat(opacityHandle, Settings::get().getHudBottomRightOpacity()));
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(0));
  rect(0.8f, 0.8f, 0.2f, 0.2f, 0.8f + 0.2f * iscale, 0.8f + 0.2f * iscale, 0.2f * scale,
       0.2f * scale);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
  // center
  ThrowIfFailed(effect->SetFloat(opacityHandle, 1.0f));
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(0));
  rect(0.37f, 0.22f, 0.4f, 0.5f, 0.37f + 0.15f * iscale, 0.22f + 0.15f * iscale, 0.4f * scale,
       0.5f * scale);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
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
  ThrowIfFailed(device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])));
}
