#include "GAUSS.h"
#include "util.h"
#include <array>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

GAUSS::GAUSS(IDirect3DDevice9* device, int width, int height) noexcept
    : Effect(device), width(width), height(height) {
  try {
    spdlog::info("Gauss construct");
    // Setup pixel size macro
    std::string pixelSize = fmt::format("float2(1.0 / {}, 1.0 / {})", width, height);
    // Setup the defines for compiling the effect
    std::array<D3DXMACRO, 2> defines = {{{"PIXEL_SIZE", pixelSize.c_str()}, {nullptr, nullptr}}};
    // Load effect from file
    spdlog::info("Gauss load");
    ID3DXBufferPtr errors;
    fs::path srcfile = GetModuleDirectoryPath() / L"dsfix\\GAUSS.fx";
    HRESULT hr = ::D3DXCreateEffectFromFileW(device, srcfile.c_str(), &defines.front(), nullptr,
                                             D3DXFX_NOT_CLONEABLE, nullptr, &effect, &errors);
    if (FAILED(hr)) {
      spdlog::error("ERRORS:");
      spdlog::error(" {}", errors->GetBufferPointer());
    }
    // Create buffers
    throw_if_fail(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer1Tex, nullptr));
    throw_if_fail(buffer1Tex->GetSurfaceLevel(0, &buffer1Surf));
    // get handles
    frameTexHandle = effect->GetParameterByName(nullptr, "frameTex2D");
  } catch (const std::system_error& err) {
    spdlog::error("{}", err.what());
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
    spdlog::error("{}", err.what());
  }
}
