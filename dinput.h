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
  HRESULT WINAPI QueryInterface(REFIID iid, void** ppvObject);
  ULONG WINAPI AddRef(void);
  ULONG WINAPI Release(void);
  HRESULT WINAPI ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback,
                                  LPDICONFIGUREDEVICESPARAMS lpdiCDParams, DWORD dwFlags,
                                  LPVOID pvRefData);
  HRESULT WINAPI CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8* lpDirectInputDevice,
                              LPUNKNOWN pUnkOuter);
  HRESULT WINAPI EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, LPVOID pvRef,
                             DWORD dwFlags);
  HRESULT WINAPI EnumDevicesBySemantics(LPCTSTR ptszUserName, LPDIACTIONFORMAT lpdiActionFormat,
                                        LPDIENUMDEVICESBYSEMANTICSCB lpCallback, LPVOID pvRef,
                                        DWORD dwFlags);
  HRESULT WINAPI FindDevice(REFGUID rguidClass, LPCTSTR ptszName, LPGUID pguidInstance);
  HRESULT WINAPI GetDeviceStatus(REFGUID rguidInstance);
  HRESULT WINAPI Initialize(HINSTANCE hinst, DWORD dwVersion);
  HRESULT WINAPI RunControlPanel(HWND hwndOwner, DWORD dwFlags);

  hkIDirectInput8(IDirectInput8* actualDirectInput);
  virtual ~hkIDirectInput8(void);
};

class hkIDirectInputDevice8 : public IDirectInputDevice8 {
private:
  IDirectInputDevice8* m_pDevice;

public:
  HRESULT WINAPI QueryInterface(REFIID iid, void** ppvObject);
  ULONG WINAPI AddRef(void);
  ULONG WINAPI Release(void);
  HRESULT WINAPI Acquire();
  HRESULT WINAPI BuildActionMap(LPDIACTIONFORMAT lpdiaf, LPCTSTR lpszUserName, DWORD dwFlags);
  HRESULT WINAPI CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT* ppdeff,
                              LPUNKNOWN punkOuter);
  HRESULT WINAPI EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback,
                                          LPVOID pvRef, DWORD fl);
  HRESULT WINAPI EnumEffects(LPDIENUMEFFECTSCALLBACK lpCallback, LPVOID pvRef, DWORD dwEffType);
  HRESULT WINAPI EnumEffectsInFile(LPCWSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec,
                                   LPVOID pvRef, DWORD dwFlags);
  HRESULT WINAPI EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD dwFlags);
  HRESULT WINAPI Escape(LPDIEFFESCAPE pesc);
  HRESULT WINAPI GetCapabilities(LPDIDEVCAPS lpDIDevCaps);
  HRESULT WINAPI GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut,
                               DWORD dwFlags);
  HRESULT WINAPI GetDeviceInfo(LPDIDEVICEINSTANCE pdidi);
  HRESULT WINAPI GetDeviceState(DWORD cbData, LPVOID lpvData);
  HRESULT WINAPI GetEffectInfo(LPDIEFFECTINFO pdei, REFGUID rguid);
  HRESULT WINAPI GetForceFeedbackState(LPDWORD pdwOut);
  HRESULT WINAPI GetImageInfo(LPDIDEVICEIMAGEINFOHEADER lpdiDevImageInfoHeader);
  HRESULT WINAPI GetObjectInfo(LPDIDEVICEOBJECTINSTANCE pdidoi, DWORD dwObj, DWORD dwHow);
  HRESULT WINAPI GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph);
  HRESULT WINAPI Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid);
  HRESULT WINAPI Poll();
  HRESULT WINAPI RunControlPanel(HWND hwndOwner, DWORD dwFlags);
  HRESULT WINAPI SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut,
                                DWORD fl);
  HRESULT WINAPI SendForceFeedbackCommand(DWORD dwFlags);
  HRESULT WINAPI SetActionMap(LPDIACTIONFORMAT lpdiActionFormat, LPCTSTR lptszUserName,
                              DWORD dwFlags);
  HRESULT WINAPI SetCooperativeLevel(HWND hwnd, DWORD dwFlags);
  HRESULT WINAPI SetDataFormat(LPCDIDATAFORMAT lpdf);
  HRESULT WINAPI SetEventNotification(HANDLE hEvent);
  HRESULT WINAPI SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph);
  HRESULT WINAPI Unacquire();
  HRESULT WINAPI WriteEffectToFile(LPCWSTR lpszFileName, DWORD dwEntries,
                                   LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);

  hkIDirectInputDevice8(IDirectInputDevice8* actual);
  virtual ~hkIDirectInputDevice8(void);
};
