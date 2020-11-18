#include "GAUSS.h"
#include "util.h"
#include <array>
#include <filesystem>
#include <string>
#include <wrl.h>

using namespace Microsoft;

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
    WRL::ComPtr<ID3DXBuffer> errors;
    fs::path srcfile = GetModuleDirectoryPath() / L"dsfix\\GAUSS.fx";
    HRESULT hr = ::D3DXCreateEffectFromFileW(device, srcfile.c_str(), &defines.front(), nullptr,
                                             D3DXFX_NOT_CLONEABLE, nullptr, &effect, &errors);
    if (FAILED(hr)) {
      spdlog::error("ERRORS:");
      spdlog::error(" {}", errors->GetBufferPointer());
    }
    // Create buffers
    ThrowIfFailed(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &buffer1Tex, nullptr));
    ThrowIfFailed(buffer1Tex->GetSurfaceLevel(0, &buffer1Surf));
    // get handles
    frameTexHandle = effect->GetParameterByName(nullptr, "frameTex2D");
  } catch (const std::system_error& err) {
    spdlog::error(L"GAUSS::GAUSS: error: {}", DXGetErrorString9W(err.code().value()));
  }
}

void GAUSS::go(IDirect3DTexture9* input, IDirect3DSurface9* dst) noexcept {
  try {
    ThrowIfFailed(device->SetVertexDeclaration(vertexDeclaration.Get()));
    UINT passes;
    // Horizontal blur
    ThrowIfFailed(device->SetRenderTarget(0, buffer1Surf.Get()));
    ThrowIfFailed(effect->SetTexture(frameTexHandle, input));
    ThrowIfFailed(effect->Begin(&passes, 0));
    ThrowIfFailed(effect->BeginPass(0));
    quad(width, height);
    ThrowIfFailed(effect->EndPass());
    ThrowIfFailed(effect->End());
    // Vertical blur
    ThrowIfFailed(device->SetRenderTarget(0, dst));
    ThrowIfFailed(effect->SetTexture(frameTexHandle, buffer1Tex.Get()));
    ThrowIfFailed(effect->Begin(&passes, 0));
    ThrowIfFailed(effect->BeginPass(1));
    quad(width, height);
    ThrowIfFailed(effect->EndPass());
    ThrowIfFailed(effect->End());
  } catch (const std::system_error& err) {
    spdlog::error(L"{}", DXGetErrorString9W(err.code().value()));
  }
}
