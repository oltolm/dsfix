#include <MinHook.h>
#include "dinput.h"
#include <cstring>

extern bool g_open;

decltype(DirectInput8Create)* oDirectInput8Create;

extern "C" {
HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
                                  LPUNKNOWN punkOuter) {

  HRESULT res = oDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
  if (FAILED(res))
    return res;

  IDirectInput8* pDinput = new hkIDirectInput8((IDirectInput8*)*ppvOut);
  *ppvOut = pDinput;

  return res;
}
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::QueryInterface(REFIID iid, void** ppvObject) {
  return m_pDinput->QueryInterface(iid, ppvObject);
}

ULONG COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::AddRef(void) { return m_pDinput->AddRef(); }

ULONG COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::Release(void) { return m_pDinput->Release(); }

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK lpdiCallback,
                                                 LPDICONFIGUREDEVICESPARAMS lpdiCDParams,
                                                 DWORD dwFlags, LPVOID pvRefData) {
  return m_pDinput->ConfigureDevices(lpdiCallback, lpdiCDParams, dwFlags, pvRefData);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::CreateDevice(REFGUID rguid,
                                             LPDIRECTINPUTDEVICE8* lpDirectInputDevice,
                                             LPUNKNOWN pUnkOuter) {
  HRESULT res = m_pDinput->CreateDevice(rguid, lpDirectInputDevice, pUnkOuter);
  if (FAILED(res))
    return res;

  IDirectInputDevice8* pDevice = new hkIDirectInputDevice8(*lpDirectInputDevice);
  *lpDirectInputDevice = pDevice;

  return res;
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACK lpCallback,
                                            LPVOID pvRef, DWORD dwFlags) {
  return m_pDinput->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::EnumDevicesBySemantics(LPCTSTR ptszUserName,
                                                       LPDIACTIONFORMAT lpdiActionFormat,
                                                       LPDIENUMDEVICESBYSEMANTICSCB lpCallback,
                                                       LPVOID pvRef, DWORD dwFlags) {
  return m_pDinput->EnumDevicesBySemantics(ptszUserName, lpdiActionFormat, lpCallback, pvRef,
                                           dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::FindDevice(REFGUID rguidClass, LPCTSTR ptszName,
                                           LPGUID pguidInstance) {
  return m_pDinput->FindDevice(rguidClass, ptszName, pguidInstance);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::GetDeviceStatus(REFGUID rguidInstance) {
  return m_pDinput->GetDeviceStatus(rguidInstance);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::Initialize(HINSTANCE hinst, DWORD dwVersion) {
  return m_pDinput->Initialize(hinst, dwVersion);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInput8::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  return m_pDinput->RunControlPanel(hwndOwner, dwFlags);
}

hkIDirectInput8::hkIDirectInput8(IDirectInput8* pDinput) { m_pDinput = pDinput; }

hkIDirectInput8::~hkIDirectInput8() {}

HRESULT COM_DECLSPEC_NOTHROW COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::QueryInterface(REFIID iid, void** ppvObject) {
  return m_pDevice->QueryInterface(iid, ppvObject);
}

ULONG COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::AddRef(void) { return m_pDevice->AddRef(); }

ULONG COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::Release(void) { return m_pDevice->Release(); }

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::Acquire() { return m_pDevice->Acquire(); }

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::BuildActionMap(LPDIACTIONFORMAT lpdiaf, LPCTSTR lpszUserName,
                                                     DWORD dwFlags) {
  return m_pDevice->BuildActionMap(lpdiaf, lpszUserName, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff,
                                                   LPDIRECTINPUTEFFECT* ppdeff,
                                                   LPUNKNOWN punkOuter) {
  return m_pDevice->CreateEffect(rguid, lpeff, ppdeff, punkOuter);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::EnumCreatedEffectObjects(
    LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl) {
  return m_pDevice->EnumCreatedEffectObjects(lpCallback, pvRef, fl);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::EnumEffects(LPDIENUMEFFECTSCALLBACK lpCallback, LPVOID pvRef,
                                                  DWORD dwEffType) {
  return m_pDevice->EnumEffects(lpCallback, pvRef, dwEffType);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::EnumEffectsInFile(LPCWSTR lpszFileName,
                                                        LPDIENUMEFFECTSINFILECALLBACK pec,
                                                        LPVOID pvRef, DWORD dwFlags) {
  return m_pDevice->EnumEffectsInFile(lpszFileName, pec, pvRef, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACK lpCallback,
                                                  LPVOID pvRef, DWORD dwFlags) {
  return m_pDevice->EnumObjects(lpCallback, pvRef, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::Escape(LPDIEFFESCAPE pesc) { return m_pDevice->Escape(pesc); }

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetCapabilities(LPDIDEVCAPS lpDIDevCaps) {
  return m_pDevice->GetCapabilities(lpDIDevCaps);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod,
                                                    LPDWORD pdwInOut, DWORD dwFlags) {
  return m_pDevice->GetDeviceData(cbObjectData, rgdod, pdwInOut, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetDeviceInfo(LPDIDEVICEINSTANCE pdidi) {
  return m_pDevice->GetDeviceInfo(pdidi);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetDeviceState(DWORD cbData, LPVOID lpvData) {
  auto ret = m_pDevice->GetDeviceState(cbData, lpvData);
  if (FAILED(ret))
    return ret;
  if (g_open)
    std::memset(lpvData, 0, cbData);
  return ret;
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetEffectInfo(LPDIEFFECTINFO pdei, REFGUID rguid) {
  return m_pDevice->GetEffectInfo(pdei, rguid);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetForceFeedbackState(LPDWORD pdwOut) {
  return m_pDevice->GetForceFeedbackState(pdwOut);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI
hkIDirectInputDevice8::GetImageInfo(LPDIDEVICEIMAGEINFOHEADER lpdiDevImageInfoHeader) {
  return m_pDevice->GetImageInfo(lpdiDevImageInfoHeader);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetObjectInfo(LPDIDEVICEOBJECTINSTANCE pdidoi, DWORD dwObj,
                                                    DWORD dwHow) {
  return m_pDevice->GetObjectInfo(pdidoi, dwObj, dwHow);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph) {
  return m_pDevice->GetProperty(rguidProp, pdiph);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid) {
  return m_pDevice->Initialize(hinst, dwVersion, rguid);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::Poll() { return m_pDevice->Poll(); }

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::RunControlPanel(HWND hwndOwner, DWORD dwFlags) {
  return m_pDevice->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::SendDeviceData(DWORD cbObjectData,
                                                     LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut,
                                                     DWORD fl) {
  return m_pDevice->SendDeviceData(cbObjectData, rgdod, pdwInOut, fl);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::SendForceFeedbackCommand(DWORD dwFlags) {
  return m_pDevice->SendForceFeedbackCommand(dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::SetActionMap(LPDIACTIONFORMAT lpdiActionFormat,
                                                   LPCTSTR lptszUserName, DWORD dwFlags) {
  return m_pDevice->SetActionMap(lpdiActionFormat, lptszUserName, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::SetCooperativeLevel(HWND hwnd, DWORD dwFlags) {
  return m_pDevice->SetCooperativeLevel(hwnd, dwFlags);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::SetDataFormat(LPCDIDATAFORMAT lpdf) {
  return m_pDevice->SetDataFormat(lpdf);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::SetEventNotification(HANDLE hEvent) {
  return m_pDevice->SetEventNotification(hEvent);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph) {
  return m_pDevice->SetProperty(rguidProp, pdiph);
}

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::Unacquire() { return m_pDevice->Unacquire(); }

HRESULT COM_DECLSPEC_NOTHROW WINAPI hkIDirectInputDevice8::WriteEffectToFile(LPCWSTR lpszFileName, DWORD dwEntries,
                                                        LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags) {
  return m_pDevice->WriteEffectToFile(lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}

hkIDirectInputDevice8::hkIDirectInputDevice8(IDirectInputDevice8* pDevice) : m_pDevice(pDevice) {}

hkIDirectInputDevice8::~hkIDirectInputDevice8() {}
