#include "GAUSS.h"
#include "log.h"
#include "util.h"
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

GAUSS::GAUSS(IDirect3DDevice9* device, int width, int height) noexcept
    : Effect(device), width(width), height(height) {
  try {
    SDLOG(LogLevel::Info, "Gauss construct");
    // Setup pixel size macro
    std::string pixelSize = tfm::format("float2(1.0 / %d, 1.0 / %d)", width, height);
    // Setup the defines for compiling the effect
    std::vector<D3DXMACRO> defines = {
        {"PIXEL_SIZE", pixelSize.c_str()}, {NULL, NULL}};
    // Load effect from file
    SDLOG(LogLevel::Info, "Gauss load");
    ID3DXBufferPtr errors;
    fs::path srcfile = GetModuleDirectoryPath() / L"dsfix\\GAUSS.fx";
    HRESULT hr = ::D3DXCreateEffectFromFileW(device, srcfile.c_str(), &defines.front(), NULL,
                                             D3DXFX_NOT_CLONEABLE, NULL, &effect, &errors);
    if (FAILED(D3D_OK)) {
      SDLOG(LogLevel::Error, "ERRORS:");
      SDLOG(LogLevel::Error, " %s", errors->GetBufferPointer());
    }
    // Create buffers
    throw_if_fail(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer1Tex, NULL));
    throw_if_fail(buffer1Tex->GetSurfaceLevel(0, &buffer1Surf));
    // get handles
    frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}

void GAUSS::go(IDirect3DTexture9* input, IDirect3DSurface9* dst) noexcept {
  try {
    throw_if_fail(device->SetVertexDeclaration(vertexDeclaration));
    UINT passes;
    // Horizontal blur
    throw_if_fail(device->SetRenderTarget(0, buffer1Surf));
    throw_if_fail(effect->SetTexture(frameTexHandle, input));
    throw_if_fail(effect->Begin(&passes, 0));
    throw_if_fail(effect->BeginPass(0));
    quad(width, height);
    throw_if_fail(effect->EndPass());
    throw_if_fail(effect->End());
    // Vertical blur
    throw_if_fail(device->SetRenderTarget(0, dst));
    throw_if_fail(effect->SetTexture(frameTexHandle, buffer1Tex));
    throw_if_fail(effect->Begin(&passes, 0));
    throw_if_fail(effect->BeginPass(1));
    quad(width, height);
    throw_if_fail(effect->EndPass());
    throw_if_fail(effect->End());
  } catch (const std::system_error& err) {
    SDLOG(LogLevel::Error, "%s", err.what());
  }
}
