#pragma once
#include <d3d9.h>
#include <wrl.h>

class RSManager;

class Ui {
  Microsoft::WRL::ComPtr<IDirect3D9> m_api;
  Microsoft::WRL::ComPtr<IDirect3DDevice9> m_device;
  void showWindow(bool* pOpen, RSManager* rsManager);

public:
  Ui(IDirect3D9* api, IDirect3DDevice9* device);
  void onEndScene(RSManager* rsManager);
  void onReset();
};
