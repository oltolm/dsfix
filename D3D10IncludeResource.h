#include <D3DX9Shader.h>
#include <Winuser.h>
#include <sstream>

extern HMODULE g_hDll;

#pragma region This stuff is for loading headers from resources
class D3D10IncludeResource : public ID3DXInclude {
    public:
        STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE, LPCSTR pFileName, LPCVOID, LPCVOID *ppData, UINT *pBytes)  {
            std::wstringstream s;
            s << pFileName;
            HRSRC src = FindResourceW(g_hDll, s.str().c_str(), RT_RCDATA);
            HGLOBAL res = LoadResource(g_hDll, src);

            *pBytes = SizeofResource(g_hDll, src);
            *ppData = (LPCVOID) LockResource(res);

            return S_OK;
        }

        STDMETHOD(Close)(THIS_ LPCVOID)  {
            return S_OK;
        }
};
#pragma endregion
