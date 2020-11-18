#pragma once
#define _COM_NO_STANDARD_GUIDS_
#include <comip.h>
#include <d3dx9.h>
#include <ostream>

_COM_SMARTPTR_TYPEDEF(ID3DXEffect, __uuidof(ID3DXEffect));
_COM_SMARTPTR_TYPEDEF(IDirect3DTexture9, __uuidof(IDirect3DTexture9));
_COM_SMARTPTR_TYPEDEF(IDirect3DSurface9, __uuidof(IDirect3DSurface9));
_COM_SMARTPTR_TYPEDEF(IDirect3DVertexDeclaration9, __uuidof(IDirect3DVertexDeclaration9));
_COM_SMARTPTR_TYPEDEF(ID3DXBuffer, __uuidof(ID3DXBuffer));
_COM_SMARTPTR_TYPEDEF(IDirect3DVertexShader9, __uuidof(IDirect3DVertexShader9));
_COM_SMARTPTR_TYPEDEF(IDirect3DPixelShader9, __uuidof(IDirect3DPixelShader9));
_COM_SMARTPTR_TYPEDEF(IDirect3DQuery9, __uuidof(IDirect3DQuery9));
_COM_SMARTPTR_TYPEDEF(IDirect3DBaseTexture9, __uuidof(IDirect3DBaseTexture9));
_COM_SMARTPTR_TYPEDEF(IDirect3DStateBlock9, __uuidof(IDirect3DStateBlock9));
_COM_SMARTPTR_TYPEDEF(IDirect3DDevice9, __uuidof(IDirect3DDevice9));
_COM_SMARTPTR_TYPEDEF(IDirect3D9, __uuidof(IDirect3D9));

template <typename T> std::ostream& operator<<(std::ostream& stream, const _com_ptr_t<T>& item) {
  stream << static_cast<void*>(item);
  return stream;
}
