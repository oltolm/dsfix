/*DSfix Copyright 2012 Peter Thoman (Durante)
thanks to Azorbix's D3D Starter Kit as well as
Matthew Fisher's Direct3D 9 API Interceptor project
for providing examples of D3D interception
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/ */
#pragma once

#include <dinput.h>

#define VERSION "2.6.0"

using DirectInput8Create_t = HRESULT(WINAPI*)(HINSTANCE inst_handle, DWORD version,
                                              const IID& r_iid, LPVOID* out_wrapper,
                                              LPUNKNOWN p_unk);
extern DirectInput8Create_t oDirectInput8Create;
void onDirect3D9Create();
