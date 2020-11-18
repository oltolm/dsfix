#pragma once
#include <d3d9.h>
const char* D3DFormatToString(D3DFORMAT format, bool bWithPrefix = true);
const char* D3DSamplerStateTypeToString(D3DSAMPLERSTATETYPE state);
const char* D3DDeclTypeToString(D3DDECLTYPE type);
const char* D3DDeclUsageToString(D3DDECLUSAGE type);
// not thread safe
const char* RectToString(const RECT* rect);
const char* D3DMatrixToString(const D3DMATRIX* pMatrix);
#ifndef D3DPRESENT_FORCEIMMEDIATE
#define D3DPRESENT_FORCEIMMEDIATE 0x00000100L
#endif