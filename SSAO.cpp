#include "SSAO.h"
#include "Settings.h"
#include "log.h"
#include "main.h"
#include "tinyformat.h"
#include "util.h"
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

SSAO::SSAO(IDirect3DDevice9* device, int width, int height, unsigned strength, Type type) noexcept
    : Effect(device), width(width), height(height) {
  try {
    // Setup pixel size macro
    std::string pixelSize = tfm::format("float2(1.0 / %d, 1.0 / %d)", width, height);
    D3DXMACRO strengthMacros[] = {
        {"SSAO_STRENGTH_LOW", "1"}, {"SSAO_STRENGTH_MEDIUM", "1"}, {"SSAO_STRENGTH_HIGH", "1"}};
    // Setup the defines for compiling the effect
    std::vector<D3DXMACRO> defines = {
        {"PIXEL_SIZE", pixelSize.c_str()}, strengthMacros[strength], {NULL, NULL}};
    DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;
    // Load effect from file
    fs::path srcfile = GetModuleDirectoryPath();
    switch (type) {
    case VSSAO:
      srcfile /= L"dsfix\\VSSAO.fx";
      break;
    case VSSAO2:
      srcfile /= L"dsfix\\VSSAO2.fx";
      break;
    }
    SDLOG(LogLevel::Info, "%s load, strength %s", srcfile, strengthMacros[strength].Name);
    ID3DXBufferPtr errors;
    HRESULT hr = ::D3DXCreateEffectFromFileW(device, srcfile.c_str(), &defines.front(), NULL, flags,
                                             NULL, &effect, &errors);
    if (FAILED(hr)) {
      SDLOG(LogLevel::Error, "ERRORS:");
      SDLOG(LogLevel::Error, " %s", errors->GetBufferPointer());
    }
    // Create buffers
    throw_if_fail(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer1Tex, NULL));
    throw_if_fail(buffer1Tex->GetSurfaceLevel(0, &buffer1Surf));
    throw_if_fail(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer2Tex, NULL));
    throw_if_fail(buffer2Tex->GetSurfaceLevel(0, &buffer2Surf));
    // get handles
    depthTexHandle = effect->GetParameterByName(NULL, "depthTex2D");
    frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
    prevPassTexHandle = effect->GetParameterByName(NULL, "prevPassTex2D");
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}

void SSAO::go(IDirect3DTexture9* frame, IDirect3DTexture9* depth, IDirect3DSurface9* dst) noexcept {
  try {
    throw_if_fail(device->SetVertexDeclaration(vertexDeclaration));

    mainSsaoPass(depth, buffer1Surf);

    for (size_t i = 0; i < 1; ++i) {
      hBlurPass(depth, buffer1Tex, buffer2Surf);
      vBlurPass(depth, buffer2Tex, buffer1Surf);
    }

    combinePass(frame, buffer1Tex, dst);
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}

void SSAO::mainSsaoPass(IDirect3DTexture9* depth, IDirect3DSurface9* dst) {
  throw_if_fail(device->SetRenderTarget(0, dst));
  throw_if_fail(device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0));
  // Setup variables.
  throw_if_fail(effect->SetTexture(depthTexHandle, depth));
  // Do it!
  UINT passes;
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(0));
  quad(width, height);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
}

void SSAO::hBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst) {
  throw_if_fail(device->SetRenderTarget(0, dst));
  // Setup variables.
  throw_if_fail(effect->SetTexture(prevPassTexHandle, src));
  throw_if_fail(effect->SetTexture(depthTexHandle, depth));
  // Do it!
  UINT passes;
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(1));
  quad(width, height);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
}

void SSAO::vBlurPass(IDirect3DTexture9* depth, IDirect3DTexture9* src, IDirect3DSurface9* dst) {
  throw_if_fail(device->SetRenderTarget(0, dst));
  // Setup variables.
  throw_if_fail(effect->SetTexture(prevPassTexHandle, src));
  throw_if_fail(effect->SetTexture(depthTexHandle, depth));
  // Do it!
  UINT passes;
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(2));
  quad(width, height);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
}

void SSAO::combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst) {
  throw_if_fail(device->SetRenderTarget(0, dst));
  // device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 255), 1.0f, 0);
  // Setup variables.
  throw_if_fail(effect->SetTexture(prevPassTexHandle, ao));
  throw_if_fail(effect->SetTexture(frameTexHandle, frame));
  // Do it!
  UINT passes;
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(3));
  quad(width, height);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
}
