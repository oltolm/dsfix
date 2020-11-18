#pragma once
#include "comptr.h"
#include "log.h"
#include "util.h"
#include <d3d9.h>

// Base class for effects
class Effect {
protected:
  IDirect3DDevice9Ptr device;
  IDirect3DVertexDeclaration9Ptr vertexDeclaration;
  static const D3DVERTEXELEMENT9 vertexElements[3];

  Effect(IDirect3DDevice9* device) noexcept : device(device) {
    try {
      throw_if_fail(device->CreateVertexDeclaration(vertexElements, &vertexDeclaration));
    } catch (const std::system_error &err) {
      SDLOG(LogLevel::Error, "Effect: %s", err.what());
    }
  }

  virtual ~Effect() = default;

  void quad(int width, int height) {
    // Draw aligned fullscreen quad
    D3DXVECTOR2 pixelSize = D3DXVECTOR2(1.0f / float(width), 1.0f / float(height));
    float quad[4][5] = {{-1.0f - pixelSize.x, 1.0f + pixelSize.y, 0.5f, 0.0f, 0.0f},
                        {1.0f - pixelSize.x, 1.0f + pixelSize.y, 0.5f, 1.0f, 0.0f},
                        {-1.0f - pixelSize.x, -1.0f + pixelSize.y, 0.5f, 0.0f, 1.0f},
                        {1.0f - pixelSize.x, -1.0f + pixelSize.y, 0.5f, 1.0f, 1.0f}};
    throw_if_fail(device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0])));
  }
};
