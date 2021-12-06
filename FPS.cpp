#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
// Dark Souls FPS fix by Clement Barnier (Nwks)
#include "FPS.h"
#include "RenderstateManager.h"
#include "Settings.h"
#include "log.h"
#include "memory.h"
#include <MinHook.h>
#include <windows.h>

// Hook Globals
static double lastRenderTime;
static LARGE_INTEGER timerFreq;
static LARGE_INTEGER counterAtStart;

// Time-step value address
// search pattern
const char* TS_PATTERN = "0080264400009444000058420000C0428988083D0000A044";
// code offset
const DWORD TS_OFFSET = 0x00000010;
//
// 011E4D50 - 00 80 26440000        - add [eax+00004426],al
// 011E4D56 - 94                    - xchg eax,esp
// 011E4D57 - 44                    - inc esp
// 011E4D58 - 00 00                 - add [eax],al
// 011E4D5A - 58                    - pop eax
// 011E4D5B - 42                    - inc edx
// 011E4D5C - 00 00                 - add [eax],al
// 011E4D5E - C0 42 89 88           - rol byte ptr [edx-77],-78 { 136 }
// 011E4D62 - 08 3D 0000A044        - or [44A00000],bh { 1280.00 }
static DWORD ADDR_TS = 0x011E4D60;

// Presentation interval address
// search pattern
const char* PRESINT_PATTERN = "FF15xxxxxxxx83C408C78648020000020000005EC20800";
// code offset
const DWORD PRESINT_OFFSET = 0x0000000F;
//
// 00FFA2FF - FF 15 ACFB1501        - call dword ptr [0115FBAC] { ->006E9C00 }
// 00FFA305 - 83 C4 08              - add esp,08 { 8 }
// 00FFA308 - C7 86 48020000 02000000 - mov [esi+00000248],00000002 { 2 }
// 00FFA312 - 5E                    - pop esi
// 00FFA313 - C2 0800               - ret 0008 { 8 }
static DWORD ADDR_PRESINT = 0x00FFA30E;

// getDrawThreadMsgCommand address in HGCommandDispatcher loop
// search pattern
const char* GETCMD_PATTERN = "6A018BCDE8xxxxxxxx8BF08BCEE8xxxxxxxx83F805";
// code offset
const DWORD GETCMD_OFFSET = 0x0000000D;
//
// 00BAC4D0 - 6A 01                 - push 01 { 1 }
// 00BAC4D2 - 8B CD                 - mov ecx,ebp
// 00BAC4D4 - E8 C7BB9CFF           - call 005780A0
// 00BAC4D9 - 8B F0                 - mov esi,eax
// 00BAC4DB - 8B CE                 - mov ecx,esi
// 00BAC4DD - E8 5EBA9CFF           - call 00577F40
// 00BAC4E2 - 83 F8 05              - cmp eax,05 { 5 }
//
// code at 00BAC4DD calls the function:
// 00577F40 - 8B 41 0C              - mov eax,[ecx+0C]
// 00577F43 - C3                    - ret
//
// in C:
// __attribute__((fastcall)) unsigned getDrawThreadMsgCommand(unsigned* cmd) {
//   return cmd[3];
// }
static DWORD ADDR_GETCMD = 0x00577F40;

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
  ADDR_TS = GetMemoryAddressFromPattern(nullptr, TS_PATTERN, TS_OFFSET);
  SDLOG(LogLevel::Info, "found time-step address at 0x%X", ADDR_TS);
  ADDR_PRESINT = GetMemoryAddressFromPattern(nullptr, PRESINT_PATTERN, PRESINT_OFFSET);
  SDLOG(LogLevel::Info, "found presentation interval address at 0x%X", ADDR_PRESINT);
  ADDR_GETCMD = GetMemoryAddressFromPattern(nullptr, GETCMD_PATTERN, GETCMD_OFFSET);
  SDLOG(LogLevel::Info, "found getDrawThreadMsgCommand address at 0x%X", ADDR_GETCMD);
  // Binary patches
  // Override D3D Presentation Interval
  const DWORD data = 5; // Set to immediate
  writeToAddress(&data, reinterpret_cast<void*>(ADDR_PRESINT), sizeof(data));
  // Detour call to getDrawThreadMsgCommand
  MH_CreateHook(reinterpret_cast<LPVOID>(ADDR_GETCMD),
                reinterpret_cast<LPVOID>(hkGetDrawThreadMsgCommand), nullptr);
  MH_EnableHook(reinterpret_cast<LPVOID>(ADDR_GETCMD));
  SDLOG(LogLevel::Info, "FPS unlocked");
}

void removeFPSHook() {
  MH_DisableHook(reinterpret_cast<LPVOID>(ADDR_GETCMD));
  MH_RemoveHook(reinterpret_cast<LPVOID>(ADDR_GETCMD));
  SDLOG(LogLevel::Info, "FPS hook removed");
}

void initFPSTimer() {
  // Init counter for frame-rate calculations
  lastRenderTime = 0.0f;
  ::QueryPerformanceFrequency(&timerFreq);
  ::QueryPerformanceCounter(&counterAtStart);
}
#pragma GCC diagnostic pop
