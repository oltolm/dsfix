#include "d3d9query.h"
#include "RenderstateManager.h"

hkIDirect3DQuery9::hkIDirect3DQuery9(IDirect3DQuery9* pIDirect3DQuery9)
    : m_pD3Dquery(pIDirect3DQuery9) {}

HRESULT APIENTRY hkIDirect3DQuery9::QueryInterface(REFIID riid, void** ppvObj) {
  return m_pD3Dquery->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3DQuery9::AddRef() { return m_pD3Dquery->AddRef(); }

ULONG APIENTRY hkIDirect3DQuery9::Release() { return m_pD3Dquery->Release(); }

HRESULT APIENTRY hkIDirect3DQuery9::GetDevice(IDirect3DDevice9** ppDevice) {
  return m_pD3Dquery->GetDevice(ppDevice);
}

D3DQUERYTYPE APIENTRY hkIDirect3DQuery9::GetType() { return m_pD3Dquery->GetType(); }

DWORD APIENTRY hkIDirect3DQuery9::GetDataSize() { return m_pD3Dquery->GetDataSize(); }

HRESULT APIENTRY hkIDirect3DQuery9::Issue(DWORD dwIssueFlags) {
  return m_pD3Dquery->Issue(dwIssueFlags);
}

HRESULT APIENTRY hkIDirect3DQuery9::GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags) {
  auto result = m_pD3Dquery->GetData(pData, dwSize, dwGetDataFlags);
  if (SUCCEEDED(result)) {
    auto pixelsDrawn = static_cast<DWORD*>(pData);
    *pixelsDrawn = static_cast<DWORD>(*pixelsDrawn / RSManager::get().getOcclusionScale());
  }
  return result;
}
