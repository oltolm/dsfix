#include "d3d9dev.h"
#include "d3d9query.h"
#include "RenderstateManager.h"
#include "Settings.h"
#include "ui.h"
#include "WindowManager.h"
#include <spdlog/spdlog.h>
#ifndef _MSC_VER
#include <dxerr9.h>
#endif

hkIDirect3DDevice9::hkIDirect3DDevice9(IDirect3DDevice9* pDevice, IDirect3D9* pD3D9)
    : m_pDevice(pDevice), m_pD3D9(pD3D9) {}

HRESULT APIENTRY hkIDirect3DDevice9::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect,
                                             HWND hDestWindowOverride,
                                             CONST RGNDATA* pDirtyRegion) {
  WindowManager::get().toggleCursorCapture();
  return m_pRSManager->redirectPresent(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister,
                                                              CONST float* pConstantData,
                                                              UINT Vector4fCount) {
  return m_pDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex,
                                                     IDirect3DSurface9* pRenderTarget) {
  if (RenderTargetIndex != 0)
    return D3D_OK; // rendertargets > 0 are not actually used by the game - this
                   // makes the log shorter
  return m_pRSManager->redirectSetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pvShader) {
  return m_pDevice->SetVertexShader(pvShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9* pViewport) {
  Settings::get().init();
  m_pRSManager->setViewport(*pViewport);
  return m_pDevice->SetViewport(pViewport);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type,
                                                          INT BaseVertexIndex, UINT MinVertexIndex,
                                                          UINT NumVertices, UINT startIndex,
                                                          UINT primCount) {
  return m_pDevice->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices,
                                         startIndex, primCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawIndexedPrimitiveUP(
    D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount,
    CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride) {
  return m_pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinIndex, NumVertices, PrimitiveCount,
                                           pIndexData, IndexDataFormat, pVertexStreamZeroData,
                                           VertexStreamZeroStride);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex,
                                                   UINT PrimitiveCount) {
  return m_pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,
                                                     UINT PrimitiveCount,
                                                     CONST void* pVertexStreamZeroData,
                                                     UINT VertexStreamZeroStride) {
  return m_pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData,
                                    VertexStreamZeroStride);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawRectPatch(UINT Handle, CONST float* pNumSegs,
                                                   CONST D3DRECTPATCH_INFO* pRectPatchInfo) {
  return m_pDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawTriPatch(UINT Handle, CONST float* pNumSegs,
                                                  CONST D3DTRIPATCH_INFO* pTriPatchInfo) {
  return m_pDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer,
                                                   D3DBACKBUFFER_TYPE Type,
                                                   IDirect3DSurface9** ppBackBuffer) {
  return m_pDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT APIENTRY hkIDirect3DDevice9::EndScene() {
  m_pUi->onEndScene();
  return m_pDevice->EndScene();
}

HRESULT APIENTRY hkIDirect3DDevice9::QueryInterface(REFIID riid, LPVOID* ppvObj) {
  return m_pDevice->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3DDevice9::AddRef() { return m_pDevice->AddRef(); }

HRESULT APIENTRY hkIDirect3DDevice9::BeginScene() { return m_pDevice->BeginScene(); }

HRESULT APIENTRY hkIDirect3DDevice9::BeginStateBlock() { return m_pDevice->BeginStateBlock(); }

HRESULT APIENTRY hkIDirect3DDevice9::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags,
                                           D3DCOLOR Color, float Z, DWORD Stencil) {
  return m_pDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT APIENTRY hkIDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect,
                                               D3DCOLOR color) {
  return m_pDevice->ColorFill(pSurface, pRect, color);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateAdditionalSwapChain(
    D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** ppSwapChain) {
  return m_pDevice->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage,
                                                       D3DFORMAT Format, D3DPOOL Pool,
                                                       IDirect3DCubeTexture9** ppCubeTexture,
                                                       HANDLE* pSharedHandle) {
  return m_pDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture,
                                      pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateDepthStencilSurface(
    UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
    DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
  if (Width == 1024 && Height == 720) {
    return m_pDevice->CreateDepthStencilSurface(
        Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), Format, MultiSample,
        MultisampleQuality, Discard, ppSurface, pSharedHandle);
  }
  return m_pDevice->CreateDepthStencilSurface(
      Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format,
                                                       D3DPOOL Pool,
                                                       IDirect3DIndexBuffer9** ppIndexBuffer,
                                                       HANDLE* pSharedHandle) {
  return m_pDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height,
                                                                 D3DFORMAT Format, D3DPOOL Pool,
                                                                 IDirect3DSurface9** ppSurface,
                                                                 HANDLE* pSharedHandle) {
  return m_pDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface,
                                                pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction,
                                                       IDirect3DPixelShader9** ppShader) {
  return m_pDevice->CreatePixelShader(pFunction, ppShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) {
  auto result = m_pDevice->CreateQuery(Type, ppQuery);
  if (Type == D3DQUERYTYPE_OCCLUSION && result == D3D_OK) {
    *ppQuery = new hkIDirect3DQuery9(*ppQuery, this);
    // These instances will leak, but there are only a set number of them
    // created
  }
  return result;
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format,
                                                        D3DMULTISAMPLE_TYPE MultiSample,
                                                        DWORD MultisampleQuality, BOOL Lockable,
                                                        IDirect3DSurface9** ppSurface,
                                                        HANDLE* pSharedHandle) {
  if (Width == 1024 && Height == 720) {
    return m_pDevice->CreateRenderTarget(Settings::get().getRenderWidth(),
                                         Settings::get().getRenderHeight(), Format, MultiSample,
                                         MultisampleQuality, Lockable, ppSurface, pSharedHandle);
  }
  return m_pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality,
                                       Lockable, ppSurface, pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,
                                                      IDirect3DStateBlock9** ppSB) {
  return m_pDevice->CreateStateBlock(Type, ppSB);
}

void getDofRes(UINT inW, UINT inH, UINT& outW, UINT& outH) {
  if (Settings::get().getDOFOverrideResolution() == 0) {
    outW = inW;
    outH = inH;
    return;
  } else {
    UINT topWidth = Settings::get().getDOFOverrideResolution() * 16 / 9,
         topHeight = Settings::get().getDOFOverrideResolution();
    outW = topWidth / (360 / inH);
    outH = topHeight / (360 / inH);
  }
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels,
                                                   DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                   IDirect3DTexture9** ppTexture,
                                                   HANDLE* pSharedHandle) {
  if (Width == 1024 && Height == 720) {
    return m_pDevice->CreateTexture(Settings::get().getRenderWidth(),
                                    Settings::get().getRenderHeight(), Levels, Usage, Format, Pool,
                                    ppTexture, pSharedHandle);
  }
  if ((Width == 512 && Height == 360) || (Width == 256 && Height == 180)) {
    UINT w, h;
    getDofRes(Width, Height, w, h);
    return m_pDevice->CreateTexture(w, h, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
  }
  if (Width == 1280 && Height == 720) {
    auto width = Settings::get().getPresentWidth() == 0 ? Settings::get().getRenderWidth()
                                                        : Settings::get().getPresentWidth();
    auto height = Settings::get().getPresentHeight() == 0 ? Settings::get().getRenderHeight()
                                                          : Settings::get().getPresentHeight();
    return m_pDevice->CreateTexture(width, height, Levels, Usage, Format, Pool, ppTexture,
                                    pSharedHandle);
  }
  return m_pDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture,
                                  pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF,
                                                        D3DPOOL Pool,
                                                        IDirect3DVertexBuffer9** VERTexBuffer,
                                                        HANDLE* pSharedHandle) {
  return m_pDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, VERTexBuffer, pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVertexDeclaration(
    CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) {
  return m_pDevice->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction,
                                                        IDirect3DVertexShader9** ppShader) {
  return m_pDevice->CreateVertexShader(pFunction, ppShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth,
                                                         UINT Levels, DWORD Usage, D3DFORMAT Format,
                                                         D3DPOOL Pool,
                                                         IDirect3DVolumeTexture9** ppVolumeTexture,
                                                         HANDLE* pSharedHandle) {
  return m_pDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool,
                                        ppVolumeTexture, pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::DeletePatch(UINT Handle) {
  return m_pDevice->DeletePatch(Handle);
}

HRESULT APIENTRY hkIDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB) {
  return m_pDevice->EndStateBlock(ppSB);
}

HRESULT APIENTRY hkIDirect3DDevice9::EvictManagedResources() {
  return m_pDevice->EvictManagedResources();
}

UINT APIENTRY hkIDirect3DDevice9::GetAvailableTextureMem() {
  return m_pDevice->GetAvailableTextureMem();
}

HRESULT APIENTRY hkIDirect3DDevice9::GetClipPlane(DWORD Index, float* pPlane) {
  return m_pDevice->GetClipPlane(Index, pPlane);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) {
  return m_pDevice->GetClipStatus(pClipStatus);
}

HRESULT APIENTRY
hkIDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) {
  return m_pDevice->GetCreationParameters(pParameters);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetCurrentTexturePalette(UINT* pPaletteNumber) {
  return m_pDevice->GetCurrentTexturePalette(pPaletteNumber);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) {
  return m_pDevice->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps) {
  return m_pDevice->GetDeviceCaps(pCaps);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9) {
  HRESULT hRet = m_pDevice->GetDirect3D(ppD3D9);
  if (SUCCEEDED(hRet))
    *ppD3D9 = m_pD3D9.Get();
  return hRet;
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
  return m_pDevice->GetDisplayMode(iSwapChain, pMode);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetFrontBufferData(UINT iSwapChain,
                                                        IDirect3DSurface9* pDestSurface) {
  return m_pDevice->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetFVF(DWORD* pFVF) { return m_pDevice->GetFVF(pFVF); }

void APIENTRY hkIDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) {
  m_pDevice->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
  return m_pDevice->GetIndices(ppIndexData);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9* pLight) {
  return m_pDevice->GetLight(Index, pLight);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetLightEnable(DWORD Index, BOOL* pEnable) {
  return m_pDevice->GetLightEnable(Index, pEnable);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial) {
  return m_pDevice->GetMaterial(pMaterial);
}

float APIENTRY hkIDirect3DDevice9::GetNPatchMode() { return m_pDevice->GetNPatchMode(); }

unsigned int APIENTRY hkIDirect3DDevice9::GetNumberOfSwapChains() {
  return m_pDevice->GetNumberOfSwapChains();
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
  return m_pDevice->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader) {
  return m_pDevice->GetPixelShader(ppShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister,
                                                             BOOL* pConstantData, UINT BoolCount) {
  return m_pDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister,
                                                             float* pConstantData,
                                                             UINT Vector4fCount) {
  return m_pDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData,
                                                             UINT Vector4iCount) {
  return m_pDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRasterStatus(UINT iSwapChain,
                                                     D3DRASTER_STATUS* pRasterStatus) {
  return m_pDevice->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) {
  return m_pDevice->GetRenderState(State, pValue);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRenderTarget(DWORD renderTargetIndex,
                                                     IDirect3DSurface9** ppRenderTarget) {
  return m_pDevice->GetRenderTarget(renderTargetIndex, ppRenderTarget);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* renderTarget,
                                                         IDirect3DSurface9* pDestSurface) {
  return m_pDevice->GetRenderTargetData(renderTarget, pDestSurface);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type,
                                                     DWORD* pValue) {
  return m_pDevice->GetSamplerState(Sampler, Type, pValue);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetScissorRect(RECT* pRect) {
  return m_pDevice->GetScissorRect(pRect);
}

BOOL APIENTRY hkIDirect3DDevice9::GetSoftwareVertexProcessing() {
  return m_pDevice->GetSoftwareVertexProcessing();
}

HRESULT APIENTRY hkIDirect3DDevice9::GetStreamSource(UINT StreamNumber,
                                                     IDirect3DVertexBuffer9** ppStreamData,
                                                     UINT* OffsetInBytes, UINT* pStride) {
  return m_pDevice->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) {
  return m_pDevice->GetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetSwapChain(UINT iSwapChain,
                                                  IDirect3DSwapChain9** pSwapChain) {
  return m_pDevice->GetSwapChain(iSwapChain, pSwapChain);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) {
  return m_pDevice->GetTexture(Stage, ppTexture);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetTextureStageState(DWORD Stage,
                                                          D3DTEXTURESTAGESTATETYPE Type,
                                                          DWORD* pValue) {
  return m_pDevice->GetTextureStageState(Stage, Type, pValue);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) {
  return m_pDevice->GetTransform(State, pMatrix);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
  return m_pDevice->GetVertexDeclaration(ppDecl);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader) {
  return m_pDevice->GetVertexShader(ppShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister,
                                                              BOOL* pConstantData, UINT BoolCount) {
  return m_pDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister,
                                                              float* pConstantData,
                                                              UINT Vector4fCount) {
  return m_pDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister,
                                                              int* pConstantData,
                                                              UINT Vector4iCount) {
  return m_pDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport) {
  return m_pDevice->GetViewport(pViewport);
}

HRESULT APIENTRY hkIDirect3DDevice9::LightEnable(DWORD LightIndex, BOOL bEnable) {
  return m_pDevice->LightEnable(LightIndex, bEnable);
}

HRESULT APIENTRY hkIDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,
                                                       CONST D3DMATRIX* pMatrix) {
  return m_pDevice->MultiplyTransform(State, pMatrix);
}

HRESULT APIENTRY hkIDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex,
                                                     UINT VertexCount,
                                                     IDirect3DVertexBuffer9* pDestBuffer,
                                                     IDirect3DVertexDeclaration9* pVertexDecl,
                                                     DWORD Flags) {
  return m_pDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl,
                                    Flags);
}

ULONG APIENTRY hkIDirect3DDevice9::Release() { return m_pDevice->Release(); }

HRESULT APIENTRY hkIDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {
  HRESULT hRet = m_pDevice->Reset(pPresentationParameters);
  if (SUCCEEDED(hRet)) {
    m_pUi->onReset();
    m_pRSManager->onReset();
  } else {
    spdlog::error(L"ERROR: Reset failed: code: {}, description: {}", DXGetErrorString9W(hRet),
                  DXGetErrorDescription9W(hRet));
  }
  return hRet;
}

HRESULT APIENTRY hkIDirect3DDevice9::SetClipPlane(DWORD Index, CONST float* pPlane) {
  return m_pDevice->SetClipPlane(Index, pPlane);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) {
  return m_pDevice->SetClipStatus(pClipStatus);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber) {
  return m_pDevice->SetCurrentTexturePalette(PaletteNumber);
}

void APIENTRY hkIDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags) {
  m_pDevice->SetCursorPosition(X, Y, Flags);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot,
                                                         IDirect3DSurface9* pCursorBitmap) {
  return m_pDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
  return m_pDevice->SetDepthStencilSurface(pNewZStencil);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs) {
  return m_pDevice->SetDialogBoxMode(bEnableDialogs);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetFVF(DWORD FVF) { return m_pDevice->SetFVF(FVF); }

void APIENTRY hkIDirect3DDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags,
                                               CONST D3DGAMMARAMP* pRamp) {
  m_pDevice->SetGammaRamp(iSwapChain, Flags, pRamp);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData) {
  return m_pDevice->SetIndices(pIndexData);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetLight(DWORD Index, CONST D3DLIGHT9* pLight) {
  return m_pDevice->SetLight(Index, pLight);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9* pMaterial) {
  return m_pDevice->SetMaterial(pMaterial);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetNPatchMode(float nSegments) {
  return m_pDevice->SetNPatchMode(nSegments);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,
                                                       CONST PALETTEENTRY* pEntries) {
  return m_pDevice->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader) {
  return m_pDevice->SetPixelShader(pShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,
                                                             CONST BOOL* pConstantData,
                                                             UINT BoolCount) {
  return m_pDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister,
                                                             CONST float* pConstantData,
                                                             UINT Vector4fCount) {
  static const UINT HALF_RESOLUTION_REGISTER = 164;
  static const float HALF_X_RESOLUTION = 512;
  static const float HALF_Y_RESOLUTION = 360;
  static const float RECIPROCAL_HALF_X_RESOLUTION = 1 / HALF_X_RESOLUTION;
  static const float RECIPROCAL_HALF_Y_RESOLUTION = 1 / HALF_Y_RESOLUTION;
  if (StartRegister <= HALF_RESOLUTION_REGISTER &&
      HALF_RESOLUTION_REGISTER < StartRegister + Vector4fCount) {
    size_t offset = (HALF_RESOLUTION_REGISTER - StartRegister) * 4;
    if (pConstantData[offset] == HALF_X_RESOLUTION &&
        pConstantData[offset + 1] == HALF_Y_RESOLUTION &&
        pConstantData[offset + 2] == RECIPROCAL_HALF_X_RESOLUTION &&
        pConstantData[offset + 3] == RECIPROCAL_HALF_Y_RESOLUTION) {
      size_t bufferSize = sizeof(float) * 4 * Vector4fCount;
      float* pBuffer = static_cast<float*>(alloca(bufferSize));
      std::memcpy(pBuffer, pConstantData, bufferSize);
      UINT width;
      UINT height;
      getDofRes(static_cast<UINT>(HALF_X_RESOLUTION), static_cast<UINT>(HALF_Y_RESOLUTION), width,
                height);
      pBuffer[offset] = static_cast<float>(width);
      pBuffer[offset + 1] = static_cast<float>(height);
      pBuffer[offset + 2] = 1.f / width;
      pBuffer[offset + 3] = 1.f / height;
      return m_pDevice->SetPixelShaderConstantF(StartRegister, pBuffer, Vector4fCount);
    }
  }
  return m_pDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister,
                                                             CONST int* pConstantData,
                                                             UINT Vector4iCount) {
  return m_pDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
  return m_pDevice->SetRenderState(State, Value);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type,
                                                     DWORD Value) {
  if (Settings::get().getFilteringOverride() == 2) {
    if (Type == D3DSAMP_MAXANISOTROPY)
      return m_pDevice->SetSamplerState(Sampler, Type, 16);
    if (Type == D3DSAMP_MINFILTER || Type == D3DSAMP_MAGFILTER)
      return m_pDevice->SetSamplerState(Sampler, Type, D3DTEXF_ANISOTROPIC);
  }
  return m_pDevice->SetSamplerState(Sampler, Type, Value);
}

bool operator==(const RECT& a, const RECT& b) {
  return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
}

HRESULT APIENTRY hkIDirect3DDevice9::SetScissorRect(CONST RECT* pRect) {
  // These are scissor rects used for shadow rendering, should not be suppressed:
  // SetScissorRect RECT[   0/   0/1024/1024]
  // SetScissorRect RECT[1024/   0/2048/1024]
  // SetScissorRect RECT[   0/1024/1024/2048]
  // SetScissorRect RECT[1024/1024/2048/2048]
  if (m_pRSManager->isViewport(*pRect) || *pRect == RECT{0, 0, 1024, 1024} ||
      *pRect == RECT{1024, 0, 2048, 1024} || *pRect == RECT{0, 1024, 1024, 2048} ||
      *pRect == RECT{1024, 1024, 2048, 2048}) {
    return m_pDevice->SetScissorRect(pRect);
  }
  return D3D_OK;
}

HRESULT APIENTRY hkIDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware) {
  return m_pDevice->SetSoftwareVertexProcessing(bSoftware);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetStreamSource(UINT StreamNumber,
                                                     IDirect3DVertexBuffer9* pStreamData,
                                                     UINT OffsetInBytes, UINT Stride) {
  return m_pDevice->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Divider) {
  return m_pDevice->SetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) {
  return m_pDevice->SetTexture(Stage, pTexture);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetTextureStageState(DWORD Stage,
                                                          D3DTEXTURESTAGESTATETYPE Type,
                                                          DWORD Value) {
  return m_pDevice->SetTextureStageState(Stage, Type, Value);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,
                                                  CONST D3DMATRIX* pMatrix) {
  return m_pDevice->SetTransform(State, pMatrix);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
  return m_pDevice->SetVertexDeclaration(pDecl);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,
                                                              CONST BOOL* pConstantData,
                                                              UINT BoolCount) {
  return m_pDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister,
                                                              CONST int* pConstantData,
                                                              UINT Vector4iCount) {
  return m_pDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

BOOL APIENTRY hkIDirect3DDevice9::ShowCursor(BOOL bShow) { return m_pDevice->ShowCursor(bShow); }

HRESULT APIENTRY hkIDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface,
                                                 CONST RECT* pSourceRect,
                                                 IDirect3DSurface9* pDestSurface,
                                                 CONST RECT* pDestRect,
                                                 D3DTEXTUREFILTERTYPE Filter) {
  return m_pDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT APIENTRY hkIDirect3DDevice9::TestCooperativeLevel() {
  return m_pDevice->TestCooperativeLevel();
}

HRESULT APIENTRY hkIDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface,
                                                   CONST RECT* pSourceRect,
                                                   IDirect3DSurface9* pDestinationSurface,
                                                   CONST POINT* pDestPoint) {
  return m_pDevice->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT APIENTRY hkIDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,
                                                   IDirect3DBaseTexture9* pDestinationTexture) {
  return m_pDevice->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT APIENTRY hkIDirect3DDevice9::ValidateDevice(DWORD* pNumPasses) {
  return m_pDevice->ValidateDevice(pNumPasses);
}

float hkIDirect3DDevice9::getOcclusionScale() { return m_pRSManager->getOcclusionScale(); }

void hkIDirect3DDevice9::setUi(Ui* pUi) { m_pUi = pUi; }

IDirect3DDevice9* hkIDirect3DDevice9::getDevice() { return m_pDevice.Get(); }

void hkIDirect3DDevice9::setRSManager(RSManager* pRSManager) { m_pRSManager = pRSManager; }
