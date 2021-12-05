#pragma once

#include <d3d9.h>

extern decltype(Direct3DCreate9)* oDirect3DCreate9;

void hookDirect3DCreate9();
void startDetour();
void endDetour();
