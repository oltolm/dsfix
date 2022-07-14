// Direct3D9 Interface
#include "d3d9int.h"
#include "Settings.h"
#include "d3d9dev.h"
#include "main.h"
#include <spdlog/spdlog.h>
#include <wrl.h>

using namespace Microsoft;

struct hkIDirect3DDevice9;

WRL::ComPtr<hkIDirect3DDevice9> g_pD3DDevice;

HRESULT APIENTRY hkIDirect3D9::QueryInterface(REFIID riid, void** ppvObj) {
  return m_pD3D9->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3D9::AddRef() { return m_pD3D9->AddRef(); }

HRESULT APIENTRY hkIDirect3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType,
                                                      D3DFORMAT AdapterFormat,
                                                      D3DFORMAT RenderTargetFormat,
                                                      D3DFORMAT DepthStencilFormat) {
  return m_pD3D9->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat,
                                         DepthStencilFormat);
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType,
                                                 D3DFORMAT AdapterFormat, DWORD Usage,
                                                 D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) {
  return m_pD3D9->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType,
                                                           D3DFORMAT SourceFormat,
                                                           D3DFORMAT TargetFormat) {
  return m_pD3D9->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType,
                                                          D3DFORMAT SurfaceFormat, BOOL Windowed,
                                                          D3DMULTISAMPLE_TYPE MultiSampleType,
                                                          DWORD* pQualityLevels) {
  return m_pD3D9->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed,
                                             MultiSampleType, pQualityLevels);
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType,
                                               D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat,
                                               BOOL Windowed) {
  return m_pD3D9->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);
}

HRESULT APIENTRY hkIDirect3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
                                            DWORD BehaviorFlags,
                                            D3DPRESENT_PARAMETERS* pPresentationParameters,
                                            IDirect3DDevice9** ppReturnedDeviceInterface) {
  spdlog::info("CreateDevice ------ Adapter {}", Adapter);
  if (Settings::get().getD3DAdapterOverride() >= 0) {
    spdlog::info(" - Adapter override to {}", Settings::get().getD3DAdapterOverride());
    Adapter = Settings::get().getD3DAdapterOverride();
  }
  HRESULT hRet = m_pD3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags,
                                       pPresentationParameters, ppReturnedDeviceInterface);
  if (SUCCEEDED(hRet)) {
    g_pD3DDevice = new hkIDirect3DDevice9(*ppReturnedDeviceInterface, this);
    *ppReturnedDeviceInterface = g_pD3DDevice.Get();
    onD3DCreateDevice();
  }
  return hRet;
}

HRESULT APIENTRY hkIDirect3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode,
                                                D3DDISPLAYMODE* pMode) {
  return m_pD3D9->EnumAdapterModes(Adapter, Format, Mode, pMode);
}

UINT APIENTRY hkIDirect3D9::GetAdapterCount() { return m_pD3D9->GetAdapterCount(); }

HRESULT APIENTRY hkIDirect3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) {
  return m_pD3D9->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT APIENTRY hkIDirect3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags,
                                                    D3DADAPTER_IDENTIFIER9* pIdentifier) {
  return m_pD3D9->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT APIENTRY hkIDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) {
  return m_pD3D9->GetAdapterModeCount(Adapter, Format);
}

HMONITOR APIENTRY hkIDirect3D9::GetAdapterMonitor(UINT Adapter) {
  return m_pD3D9->GetAdapterMonitor(Adapter);
}

HRESULT APIENTRY hkIDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) {
  return m_pD3D9->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HRESULT APIENTRY hkIDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction) {
  return m_pD3D9->RegisterSoftwareDevice(pInitializeFunction);
}

ULONG APIENTRY hkIDirect3D9::Release() { return m_pD3D9->Release(); }
