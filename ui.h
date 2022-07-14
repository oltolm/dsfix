#pragma once
#include <d3d9.h>
#include <wrl.h>

class RSManager;

class Ui {
  Microsoft::WRL::ComPtr<IDirect3DDevice9> m_pDevice;
  RSManager* m_pRSManager;
  void showWindow(bool* pOpen);

public:
  Ui(IDirect3DDevice9* pDevice, RSManager* pRSManager);
  void setRSManager(RSManager* p_RSManager);

  void onEndScene();
  void onReset();
};
