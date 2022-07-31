#pragma once
#include "util.h"
#include <d3dx9.h>
#include <spdlog/spdlog.h>
#ifndef _MSC_VER
#include <dxerr9.h>
#endif
#include <wrl.h>

// Base class for effects
class Effect {
protected:
  Microsoft::WRL::ComPtr<IDirect3DDevice9> device;
  Microsoft::WRL::ComPtr<IDirect3DVertexDeclaration9> vertexDeclaration;

  Effect(IDirect3DDevice9* device) noexcept : device(device) {}

  virtual ~Effect() = default;

  void quad(int width, int height) {
    // Draw aligned fullscreen quad
    D3DXVECTOR2 pixelSize = D3DXVECTOR2(1.0f / float(width), 1.0f / float(height));
    float quad[4][5] = {{-1.0f - pixelSize.x, 1.0f + pixelSize.y, 0.5f, 0.0f, 0.0f},
                        {1.0f - pixelSize.x, 1.0f + pixelSize.y, 0.5f, 1.0f, 0.0f},
                        {-1.0f - pixelSize.x, -1.0f + pixelSize.y, 0.5f, 0.0f, 1.0f},
                        {1.0f - pixelSize.x, -1.0f + pixelSize.y, 0.5f, 1.0f, 1.0f}};
    ThrowIfFailed(device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])));
  }
};
