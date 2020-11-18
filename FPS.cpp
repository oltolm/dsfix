// Dark Souls FPS fix by Clement Barnier (Nwks)
#include "FPS.h"
#include "RenderstateManager.h"
#include "Settings.h"
#include "log.h"
#include <MinHook.h>
#include <windows.h>

// Hook Globals
static double lastRenderTime;
static LARGE_INTEGER timerFreq;
static LARGE_INTEGER counterAtStart;

// Time-step value address
// 1.0.0 was 0x012497F0, 1.0.1 was 0x012498E0
const static DWORD ADDR_TS = 0x011E4D60;
// Presentation interval address
// 1.0.0 was 0x010275AE, 1.0.1 was 0x0102788E
const static DWORD ADDR_PRESINT = 0x00FFA30E;
// getDrawThreadMsgCommand address in HGCommandDispatcher loop
const static DWORD ADDR_GETCMD = 0x00577F40;

void writeToAddress(const void* Data, void* Address, size_t Size) {
  DWORD oldProtect;
  if (::VirtualProtect(Address, Size, PAGE_READWRITE, &oldProtect)) {
    ::CopyMemory(Address, Data, Size);
    ::VirtualProtect(Address, Size, oldProtect, &oldProtect);
    return;
  }
}

// Memory
void updateAnimationStepTime(float stepTime, float minFPS, float maxFPS) {
  float FPS = 1.0f / (stepTime / 1000);
  if (FPS < minFPS)
    FPS = minFPS;
  else if (FPS > maxFPS)
    FPS = maxFPS;
  float cappedStep = 1 / FPS;
  if (RSManager::get().isPaused())
    cappedStep = 0.000000000000000001f;
  writeToAddress(&cappedStep, reinterpret_cast<void*>(ADDR_TS), sizeof(cappedStep));
}

// Timer
double getElapsedTime(void) {
  LARGE_INTEGER c;
  ::QueryPerformanceCounter(&c);
  return (c.QuadPart - counterAtStart.QuadPart) * 1000.0f / timerFreq.QuadPart;
}

// Hook functions
void updateFramerate(unsigned int cmd) {
  // If rendering was performed, update animation step-time
  if ((cmd == 2) || (cmd == 5)) {
    // FPS regulation based on previous render
    double maxFPS = Settings::get().getCurrentFPSLimit();
    double minFPS = 10.0f;
    double currentTime = getElapsedTime();
    double deltaTime = currentTime - lastRenderTime;
    lastRenderTime = currentTime;
    // Update step-time
    updateAnimationStepTime(deltaTime, minFPS, maxFPS);
  }
}

__attribute__((fastcall)) unsigned int hkGetDrawThreadMsgCommand(unsigned int* cmd) {
  unsigned int ret = cmd[3];
  updateFramerate(ret);
  return ret;
}

// Game Patches
void applyFPSPatch() {
  SDLOG(LogLevel::Info, "Starting FPS unlock...");
  // Binary patches
  // Override D3D Presentation Interval
  const DWORD data = 5; // Set to immediate
  writeToAddress(&data, (void*)ADDR_PRESINT, sizeof(data));
  // Detour call to getDrawThreadMsgCommand
  MH_STATUS ret = MH_CreateHook((LPVOID)ADDR_GETCMD, (LPVOID)hkGetDrawThreadMsgCommand, nullptr);
  if (ret != MH_OK) {
    SDLOG(LogLevel::Error, "applyFPSPatch: MH_CreateHook failed: %s", MH_StatusToString(ret));
    return;
  }
  ret = MH_EnableHook((LPVOID)ADDR_GETCMD);
  if (ret != MH_OK) {
    SDLOG(LogLevel::Error, "applyFPSPatch: MH_EnableHook failed: %s", MH_StatusToString(ret));
    return;
  }
  SDLOG(LogLevel::Info, "FPS unlocked");
}

void removeFPSHook() {
  MH_STATUS ret = MH_DisableHook((LPVOID)ADDR_GETCMD);
  if (ret != MH_OK)
    return;
  ret = MH_RemoveHook((LPVOID)ADDR_GETCMD);
  if (ret != MH_OK)
    return;
  SDLOG(LogLevel::Info, "FPS hook removed");
}

void initFPSTimer() {
  // Init counter for frame-rate calculations
  lastRenderTime = 0.0f;
  ::QueryPerformanceFrequency(&timerFreq);
  ::QueryPerformanceCounter(&counterAtStart);
}
