#include "d3d9query.h"

hkIDirect3DQuery9::hkIDirect3DQuery9(IDirect3DQuery9* pIDirect3DQuery9, hkIDirect3DDevice9* pDevice)
    : m_pD3Dquery(pIDirect3DQuery9), m_device(pDevice) {}

HRESULT COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::QueryInterface(REFIID riid, void** ppvObj) {
  return m_pD3Dquery->QueryInterface(riid, ppvObj);
}

ULONG COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::AddRef() { return m_pD3Dquery->AddRef(); }

ULONG COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::Release() { return m_pD3Dquery->Release(); }

HRESULT COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::GetDevice(IDirect3DDevice9** ppDevice) {
  return m_pD3Dquery->GetDevice(ppDevice);
}

D3DQUERYTYPE COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::GetType() { return m_pD3Dquery->GetType(); }

DWORD COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::GetDataSize() { return m_pD3Dquery->GetDataSize(); }

HRESULT COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::Issue(DWORD dwIssueFlags) {
  return m_pD3Dquery->Issue(dwIssueFlags);
}

HRESULT COM_DECLSPEC_NOTHROW APIENTRY hkIDirect3DQuery9::GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags) {
  auto result = m_pD3Dquery->GetData(pData, dwSize, dwGetDataFlags);
  if (SUCCEEDED(result)) {
    auto pixelsDrawn = static_cast<DWORD*>(pData);
    *pixelsDrawn = static_cast<DWORD>(*pixelsDrawn / m_device->getOcclusionScale());
  }
  return result;
}
