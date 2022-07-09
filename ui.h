#pragma once
#include <d3d9.h>
#include <wrl.h>

class RSManager;

class Ui {
  Microsoft::WRL::ComPtr<IDirect3DDevice9> m_pDevice;
  void showWindow(bool* pOpen);

  static Ui instance;

public:
  Ui() = default;

  static Ui& get() { return instance; }

  void setD3DDevice(IDirect3DDevice9* pDevice) { m_pDevice = pDevice; }

  void onEndScene();
  void onReset();
};
