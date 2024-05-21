#include "SSAO.h"
#include "util.h"

#include <array>
#include <spdlog/formatter.h>
#include <string>

extern HMODULE g_hDll;

SSAO::SSAO(IDirect3DDevice9* device, unsigned int width, unsigned int height, unsigned strength, Type type) noexcept
    : Effect(device), width(width), height(height) {
  try {
    // Setup pixel size macro
    std::string pixelSize = fmt::format("float2(1.0 / {}, 1.0 / {})", width, height);
    D3DXMACRO strengthMacros[] = {
        {"SSAO_STRENGTH_LOW", "1"}, {"SSAO_STRENGTH_MEDIUM", "1"}, {"SSAO_STRENGTH_HIGH", "1"}};
    // Setup the defines for compiling the effect
    std::array<D3DXMACRO, 3> defines = {
        {{"PIXEL_SIZE", pixelSize.c_str()}, strengthMacros[strength], {nullptr, nullptr}}};
    DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;
    // Load effect from file
    const wchar_t* srcfile = [type]() {
      switch (type) {
      default:
      case Type::HBAO:
        return L"HBAO.fx";
      case Type::VSSAO:
        return L"VSSAO.fx";
      case Type::VSSAO2:
        return L"VSSAO2.fx";
      }
    }();
    // spdlog::info("{} load, strength {}", srcfile, strengthMacros[strength].Name);
    spdlog::info(L"{} load", srcfile);
    WRL::ComPtr<ID3DXBuffer> errors;
    ThrowIfFailed(::D3DXCreateEffectFromResourceW(device, g_hDll, srcfile, &defines.front(),
                                                  nullptr, flags, nullptr, &effect, &errors));
    // Create buffers
    ThrowIfFailed(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer1Tex, nullptr));
    ThrowIfFailed(buffer1Tex->GetSurfaceLevel(0, &buffer1Surf));
    ThrowIfFailed(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer2Tex, nullptr));
    ThrowIfFailed(buffer2Tex->GetSurfaceLevel(0, &buffer2Surf));
    // get handles
    depthTexHandle = effect->GetParameterByName(nullptr, "depthTex2D");
    frameTexHandle = effect->GetParameterByName(nullptr, "frameTex2D");
    prevPassTexHandle = effect->GetParameterByName(nullptr, "prevPassTex2D");
  } catch (const std::system_error& err) {
    spdlog::error(L"{}", DXGetErrorString9W(err.code().value()));
  }
}

void SSAO::go(IDirect3DTexture9* frame, IDirect3DTexture9* depth, IDirect3DSurface9* dst) noexcept {
  try {
    ThrowIfFailed(device->SetVertexDeclaration(vertexDeclaration.Get()));

    mainSsaoPass(depth, buffer1Surf.Get());

    for (size_t i = 0; i < 1; ++i) {
      hBlurPass(depth, buffer1Tex.Get(), buffer2Surf.Get());
      vBlurPass(depth, buffer2Tex.Get(), buffer1Surf.Get());
    }

    combinePass(frame, buffer1Tex.Get(), dst);
  } catch (const std::system_error& err) {
    spdlog::error(L"{}", DXGetErrorString9W(err.code().value()));
  }
}

void SSAO::mainSsaoPass(IDirect3DTexture9* depth, IDirect3DSurface9* dst) {
  ThrowIfFailed(device->SetRenderTarget(0, dst));
  ThrowIfFailed(device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0));
  // Setup variables.
  ThrowIfFailed(effect->SetTexture(depthTexHandle, depth));
  // Do it!
  UINT passes;
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(0));
  quad(width, height);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
}

void SSAO::hBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst) {
  ThrowIfFailed(device->SetRenderTarget(0, dst));
  // Setup variables.
  ThrowIfFailed(effect->SetTexture(prevPassTexHandle, src));
  ThrowIfFailed(effect->SetTexture(depthTexHandle, depth));
  // Do it!
  UINT passes;
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(1));
  quad(width, height);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
}

void SSAO::vBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst) {
  ThrowIfFailed(device->SetRenderTarget(0, dst));
  // Setup variables.
  ThrowIfFailed(effect->SetTexture(prevPassTexHandle, src));
  ThrowIfFailed(effect->SetTexture(depthTexHandle, depth));
  // Do it!
  UINT passes;
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(2));
  quad(width, height);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
}

void SSAO::combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst) {
  ThrowIfFailed(device->SetRenderTarget(0, dst));
  // device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 255), 1.0f, 0);
  // Setup variables.
  ThrowIfFailed(effect->SetTexture(prevPassTexHandle, ao));
  ThrowIfFailed(effect->SetTexture(frameTexHandle, frame));
  // Do it!
  UINT passes;
  ThrowIfFailed(effect->Begin(&passes, 0));
  ThrowIfFailed(effect->BeginPass(3));
  quad(width, height);
  ThrowIfFailed(effect->EndPass());
  ThrowIfFailed(effect->End());
}
