#include "FXAA.h"
#include "Settings.h"
#include "log.h"
#include "util.h"
#include <string>
#include <tinyformat.h>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

FXAA::FXAA(IDirect3DDevice9* device, int width, int height, Quality quality) noexcept
    : Effect(device), width(width), height(height) {
  try {
    // Setup the defines for compiling the effect
    std::vector<D3DXMACRO> defines;
    // Setup pixel size macro
    std::string pixelSize = tfm::format("float2(1.0 / %d, 1.0 / %d)", width, height);
    defines.push_back({"PIXEL_SIZE", pixelSize.c_str()});
    D3DXMACRO qualityMacros[] = {{"FXAA_QUALITY__PRESET", "10"},
                                 {"FXAA_QUALITY__PRESET", "20"},
                                 {"FXAA_QUALITY__PRESET", "28"},
                                 {"FXAA_QUALITY__PRESET", "39"}};
    defines.push_back(qualityMacros[(int)quality]);
    defines.push_back({NULL, NULL});
    // Load effect from file
    SDLOG(LogLevel::Info, "FXAA load");
    ID3DXBufferPtr errors;
    fs::path srcfile = GetModuleDirectoryPath() / L"dsfix\\FXAA.fx";
    HRESULT hr = ::D3DXCreateEffectFromFileW(device, srcfile.c_str(), &defines[0], NULL,
                                             D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3,
                                             NULL, &effect, &errors);
    if (FAILED(hr))
      SDLOG(LogLevel::Error, "ERRORS:\n %s", errors->GetBufferPointer());
    // Create buffer
    throw_if_fail(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer1Tex, NULL));
    throw_if_fail(buffer1Tex->GetSurfaceLevel(0, &buffer1Surf));
    // get handles
    frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}

void FXAA::go(IDirect3DTexture9* frame, IDirect3DSurface9* dst) noexcept {
  try {
    throw_if_fail(device->SetVertexDeclaration(vertexDeclaration));
    lumaPass(frame, buffer1Surf);
    fxaaPass(buffer1Tex, dst);
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}

void FXAA::lumaPass(IDirect3DTexture9* frame, IDirect3DSurface9* dst) {
  throw_if_fail(device->SetRenderTarget(0, dst));
  // Setup variables
  throw_if_fail(effect->SetTexture(frameTexHandle, frame));
  // Do it!
  UINT passes;
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(0));
  quad(width, height);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
}

void FXAA::fxaaPass(IDirect3DTexture9* src, IDirect3DSurface9* dst) {
  throw_if_fail(device->SetRenderTarget(0, dst));
  // Setup variables
  throw_if_fail(effect->SetTexture(frameTexHandle, src));
  // Do it!
  UINT passes;
  throw_if_fail(effect->Begin(&passes, 0));
  throw_if_fail(effect->BeginPass(1));
  quad(width, height);
  throw_if_fail(effect->EndPass());
  throw_if_fail(effect->End());
}
