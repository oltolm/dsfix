#pragma once
#include <d3dx9.h>

extern bool timingIntroMode;

using D3DXCreateTexture_FNType = HRESULT(WINAPI*)(_In_ LPDIRECT3DDEVICE9 pDevice, _In_ UINT Width,
                                                  _In_ UINT Height, _In_ UINT MipLevels,
                                                  _In_ DWORD Usage, _In_ D3DFORMAT Format,
                                                  _In_ D3DPOOL Pool,
                                                  _Out_ LPDIRECT3DTEXTURE9* ppTexture);

using D3DXCreateTextureFromFileInMemory_FNType =
    HRESULT(WINAPI*)(_In_ LPDIRECT3DDEVICE9 pDevice, _In_ LPCVOID pSrcData, _In_ UINT SrcDataSize,
                     _Out_ LPDIRECT3DTEXTURE9* ppTexture);

using D3DXCreateTextureFromFileInMemoryEx_FNType =
    HRESULT(WINAPI*)(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width,
                     UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                     DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo,
                     PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture);

extern D3DXCreateTextureFromFileInMemoryEx_FNType OrigD3DXCreateTextureFromFileInMemoryEx;
extern D3DXCreateTextureFromFileInMemory_FNType OrigD3DXCreateTextureFromFileInMemory;

void earlyDetour();
void startDetour();
void endDetour();
