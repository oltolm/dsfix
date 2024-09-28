#pragma once
#include <windows.h>
#include <dinput.h>

#if !defined(LPDIENUMEFFECTSCALLBACK)
DECL_WINELIB_TYPE_AW(LPDIENUMEFFECTSCALLBACK)
#endif

extern decltype(DirectInput8Create)* oDirectInput8Create;

class hkIDirectInput8 : public IDirectInput8 {
private:
  IDirectInput8* m_pDinput;

public:
    /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject);
    STDMETHOD_(ULONG,AddRef)(THIS);
    STDMETHOD_(ULONG,Release)(THIS);
    /*** IDirectInput8W methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID rguid, LPDIRECTINPUTDEVICE8W *lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
    STDMETHOD(EnumDevices)(THIS_ DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags);
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID rguidInstance);
    STDMETHOD(RunControlPanel)(THIS_ HWND hwndOwner, DWORD dwFlags);
    STDMETHOD(Initialize)(THIS_ HINSTANCE hinst, DWORD dwVersion);
    STDMETHOD(FindDevice)(THIS_ REFGUID rguid, LPCWSTR pszName, LPGUID pguidInstance);
    STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCWSTR ptszUserName, LPDIACTIONFORMATW lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCBW lpCallback, LPVOID pvRef, DWORD dwFlags);
    STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMSW lpdiCDParams, DWORD dwFlags, LPVOID pvRefData);

  hkIDirectInput8(IDirectInput8* actualDirectInput);
  virtual ~hkIDirectInput8(void);
};

class hkIDirectInputDevice8 : public IDirectInputDevice8 {
private:
  IDirectInputDevice8* m_pDevice;

public:
   /*** IUnknown methods ***/
    STDMETHOD_(HRESULT,QueryInterface)(THIS_ REFIID riid, void** ppvObject);
    STDMETHOD_(ULONG,AddRef)(THIS);
    STDMETHOD_(ULONG,Release)(THIS);
    /*** IDirectInputDeviceW methods ***/
    STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS lpDIDevCaps);
    STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags);
    STDMETHOD(GetProperty)(THIS_ REFGUID rguidProp, LPDIPROPHEADER pdiph);
    STDMETHOD(SetProperty)(THIS_ REFGUID rguidProp, LPCDIPROPHEADER pdiph);
    STDMETHOD(Acquire)(THIS);
    STDMETHOD(Unacquire)(THIS);
    STDMETHOD(GetDeviceState)(THIS_ DWORD cbData, LPVOID lpvData);
    STDMETHOD(GetDeviceData)(THIS_ DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
    STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT lpdf);
    STDMETHOD(SetEventNotification)(THIS_ HANDLE hEvent);
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND hwnd, DWORD dwFlags);
    STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow);
    STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW pdidi);
    STDMETHOD(RunControlPanel)(THIS_ HWND hwndOwner, DWORD dwFlags);
    STDMETHOD(Initialize)(THIS_ HINSTANCE hinst, DWORD dwVersion, REFGUID rguid);
    /*** IDirectInputDevice2W methods ***/
    STDMETHOD(CreateEffect)(THIS_ REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter);
    STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwEffType);
    STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFOW pdei, REFGUID rguid);
    STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD pdwOut);
    STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD dwFlags);
    STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl);
    STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE pesc);
    STDMETHOD(Poll)(THIS);
    STDMETHOD(SendDeviceData)(THIS_ DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl);
    /*** IDirectInputDevice7W methods ***/
    STDMETHOD(EnumEffectsInFile)(THIS_ LPCWSTR lpszFileName,LPDIENUMEFFECTSINFILECALLBACK pec,LPVOID pvRef,DWORD dwFlags);
    STDMETHOD(WriteEffectToFile)(THIS_ LPCWSTR lpszFileName,DWORD dwEntries,LPDIFILEEFFECT rgDiFileEft,DWORD dwFlags);
    /*** IDirectInputDevice8W methods ***/
    STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMATW lpdiaf, LPCWSTR lpszUserName, DWORD dwFlags);
    STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMATW lpdiaf, LPCWSTR lpszUserName, DWORD dwFlags);
    STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADERW lpdiDevImageInfoHeader);

  hkIDirectInputDevice8(IDirectInputDevice8* actual);
  virtual ~hkIDirectInputDevice8(void);
};
