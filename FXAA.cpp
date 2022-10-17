#include "FXAA.h"
#include "../Settings.h"
#include "../util.h"
#include <array>
#include <spdlog/formatter.h>
#include <string>
#ifndef _MSC_VER
#include <dxerr9.h>
#endif
#include "../D3D10IncludeResource.h"
#include <wrl.h>

using namespace Microsoft;

extern HMODULE g_hDll;

FXAA::FXAA(IDirect3DDevice9* device, int width, int height, Quality quality) noexcept
    : Effect(device), width(width), height(height) {
  try {
    // Setup pixel size macro
    std::string pixelSize = fmt::format("float2(1.0 / {}, 1.0 / {})", width, height);
    D3DXMACRO qualityMacros[] = {{"FXAA_QUALITY__PRESET", "10"},
                                 {"FXAA_QUALITY__PRESET", "20"},
                                 {"FXAA_QUALITY__PRESET", "28"},
                                 {"FXAA_QUALITY__PRESET", "39"}};
    // Setup the defines for compiling the effect
    std::array<D3DXMACRO, 3> defines = {
        {{"PIXEL_SIZE", pixelSize.c_str()}, qualityMacros[(int)quality], {nullptr, nullptr}}};
    // Load effect from file
    spdlog::info("FXAA load");
    WRL::ComPtr<ID3DXBuffer> errors;
    D3D10IncludeResource include;
    ThrowIfFailed(::D3DXCreateEffectFromResourceW(
        device, g_hDll, L"FXAA.fx", &defines[0], &include,
        D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3, nullptr, &effect, &errors));
    // Create buffer
    ThrowIfFailed(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer1Tex, nullptr));
    ThrowIfFailed(buffer1Tex->GetSurfaceLevel(0, &buffer1Surf));
    // get handles
    frameTexHandle = effect->GetParameterByName(nullptr, "frameTex2D");
  } catch (const std::system_error& err) {
    spdlog::error(L"{}", DXGetErrorString9W(err.code().value()));
  }
}

void FXAA::go(IDirect3DTexture9* frame, IDirect3DSurface9* dst) noexcept {
  try {
    ThrowIfFailed(device->SetVertexDeclaration(vertexDeclaration.Get()));
    lumaPass(frame, buffer1Surf.Get());
    fxaaPass(buffer1Tex.Get(), dst);
  } catch (const std::system_error& err) {
    spdlog::error(L"{}", DXGetErrorString9W(err.code().value()));
  }
}

void FXAA::lumaPass(IDirect3DTexture9* frame, IDirect3DSurface9* dst) {
  ThrowIfFailed(device->SetRenderTarget(0, dst));
  // Setup variables
  ThrowIfFailed(effect->SetTexture(frameTexHandle, frame));
  // Do it!
  UINT passes;
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(0));
  quad(width, height);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
}

void FXAA::fxaaPass(IDirect3DTexture9* src, IDirect3DSurface9* dst) {
  ThrowIfFailed(device->SetRenderTarget(0, dst));
  // Setup variables
  ThrowIfFailed(effect->SetTexture(frameTexHandle, src));
  // Do it!
  UINT passes;
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(1));
  quad(width, height);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
}
