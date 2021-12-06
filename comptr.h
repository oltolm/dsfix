#pragma once

#include <comdef.h>
#include <d3d9.h>
#include <ostream>

class ID3DXEffect;
class ID3DXBuffer;

#ifdef _MSC_VER
interface DECLSPEC_UUID("F6CEB4B3-4E4C-40dd-B883-8D8DE5EA0CD5") ID3DXEffect;
interface DECLSPEC_UUID("8BA5FB08-5195-40e2-AC58-0D989C3A0102") ID3DXBuffer;
#endif

_COM_SMARTPTR_TYPEDEF(ID3DXEffect, __uuidof(ID3DXEffect));
_COM_SMARTPTR_TYPEDEF(ID3DXBuffer, __uuidof(ID3DXBuffer));
_COM_SMARTPTR_TYPEDEF(IDirect3DTexture9, __uuidof(IDirect3DTexture9));
_COM_SMARTPTR_TYPEDEF(IDirect3DSurface9, __uuidof(IDirect3DSurface9));
_COM_SMARTPTR_TYPEDEF(IDirect3DVertexDeclaration9, __uuidof(IDirect3DVertexDeclaration9));
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
