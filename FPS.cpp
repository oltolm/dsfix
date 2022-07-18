#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
// Dark Souls FPS fix by Clement Barnier (Nwks)
#include "FPS.h"
#include "Settings.h"
#include "memory.h"
#include "minhook/src/hde/hde32.h"
#include <MinHook.h>
#include <spdlog/spdlog.h>
#include <windows.h>

bool g_paused = false;

// Hook Globals
static double lastRenderTime;
static LARGE_INTEGER timerFreq;
static LARGE_INTEGER counterAtStart;

// Time-step value address
// search pattern
static const std::string TS_PATTERN = "0080264400009444000058420000C0428988083D0000A044";
// code offset
static const DWORD TS_OFFSET = 0x00000010;
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
static PBYTE ADDR_TS = reinterpret_cast<PBYTE>(0x011E4D60);

// Presentation interval address
// search pattern
static const std::string PRESINT_PATTERN = "FF15xxxxxxxx83C408C78648020000020000005EC20800";
// code offset
static const DWORD PRESINT_OFFSET = 0x0000000F;
//
// 00FFA2FF - FF 15 ACFB1501        - call dword ptr [0115FBAC] { ->006E9C00 }
// 00FFA305 - 83 C4 08              - add esp,08 { 8 }
// 00FFA308 - C7 86 48020000 02000000 - mov [esi+00000248],00000002 { 2 }
// 00FFA312 - 5E                    - pop esi
// 00FFA313 - C2 0800               - ret 0008 { 8 }
static PBYTE ADDR_PRESINT = reinterpret_cast<PBYTE>(0x00FFA30E);

// getDrawThreadMsgCommand address in HGCommandDispatcher loop
// search pattern
static const std::string GETCMD_PATTERN = "6A018BCDE8xxxxxxxx8BF08BCEE8xxxxxxxx83F805";
// code offset
static const DWORD GETCMD_OFFSET = 0x0000000D;
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
static PBYTE ADDR_GETCMD = reinterpret_cast<PBYTE>(0x00577F40);

// Memory
void updateAnimationStepTime(float stepTime, float minFPS, float maxFPS) {
  float FPS = 1.0f / (stepTime / 1000);
  if (FPS < minFPS)
    FPS = minFPS;
  else if (FPS > maxFPS)
    FPS = maxFPS;
  float cappedStep = 1 / FPS;
  if (g_paused)
    cappedStep = 0.000000000000000001f;
  writeToAddress(&cappedStep, ADDR_TS, sizeof(cappedStep));
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
    double maxFPS = Settings::get().getFPSLimit();
    double minFPS = 10.0f;
    double currentTime = getElapsedTime();
    double deltaTime = currentTime - lastRenderTime;
    lastRenderTime = currentTime;
    // Update step-time
    updateAnimationStepTime(deltaTime, minFPS, maxFPS);
  }
}

unsigned int __fastcall hkGetDrawThreadMsgCommand(unsigned int* cmd) {
  updateFramerate(cmd[3]);
  return cmd[3];
}

// Game Patches
void applyFPSPatch() {
  spdlog::info("Starting FPS unlock...");
  ADDR_TS = GetMemoryAddressFromPattern(TS_PATTERN);
  ADDR_TS += ADDR_TS == 0 ? 0 : TS_OFFSET;
  spdlog::info("found time-step address at {:p}", (void*)ADDR_TS);
  ADDR_PRESINT = GetMemoryAddressFromPattern(PRESINT_PATTERN);
  ADDR_PRESINT += ADDR_PRESINT == 0 ? 0 : PRESINT_OFFSET;
  spdlog::info("found presentation interval address at {:p}", (void*)ADDR_PRESINT);
  PBYTE callAddress = GetMemoryAddressFromPattern(GETCMD_PATTERN);
  callAddress += callAddress == 0 ? 0 : GETCMD_OFFSET;
  hde32s hs = {};
  hde32_disasm(callAddress, &hs);
  ADDR_GETCMD = callAddress + hs.len + hs.imm.imm32;
  spdlog::info("found getDrawThreadMsgCommand address at {:p}", (void*)ADDR_GETCMD);
  // Binary patches
  // Override D3D Presentation Interval
  const DWORD data = 5; // Set to immediate
  writeToAddress(&data, ADDR_PRESINT, sizeof(data));
  // Detour call to getDrawThreadMsgCommand
  MH_CreateHook(ADDR_GETCMD, reinterpret_cast<LPVOID>(hkGetDrawThreadMsgCommand), nullptr);
  MH_EnableHook(ADDR_GETCMD);
  spdlog::info("FPS unlocked");
}

void removeFPSHook() {
  MH_DisableHook(ADDR_GETCMD);
  MH_RemoveHook(ADDR_GETCMD);
  spdlog::info("FPS hook removed");
}

void initFPSTimer() {
  // Init counter for frame-rate calculations
  lastRenderTime = 0.0f;
  ::QueryPerformanceFrequency(&timerFreq);
  ::QueryPerformanceCounter(&counterAtStart);
}

#pragma GCC diagnostic pop

void ApplyDS1Patches() {
  // Orig1: FF 24 85 XX XX XX XX D9 E8 6A 00
  // Orig2: 74 0D D9 44 24 08 51

  // DARKSOULS.exe+831383 - 74 0D                 - je DARKSOULS.exe+831392
  // DARKSOULS.exe+831385 - D9 44 24 08           - fld dword ptr [esp+08]
  // DARKSOULS.exe+831389 - 51                    - push ecx
  // DARKSOULS.exe+83138A - D9 1C 24              - fstp dword ptr [esp]
  // DARKSOULS.exe+83138D - E8 DEFDFFFF           - call DARKSOULS.exe+831170
  // DARKSOULS.exe+831392 - 5E                    - pop esi
  // DARKSOULS.exe+831393 - C2 0400               - ret 0004 { 4 }

  // unsigned short aob1[] = { 0xFF, 0x24, 0x85, 0x100, 0x100, 0x100, 0x100, 0xD9, 0xE8, 0x6A, 0x00
  // };
  unsigned char replace1[] = {0xE9, 0x27, 0x01, 0x00, 0x00, 0x90, 0x90, 0xD9, 0xE8, 0x6A, 0x00};

  // unsigned short aob2[] = { 0x74, 0x0D, 0xD9, 0x44, 0x24, 0x08, 0x51 };
  unsigned char replace2[] = {0x90, 0x90, 0xD9, 0x44, 0x24, 0x08, 0x51};

  auto addr1 = GetMemoryAddressFromPattern("FF2485XXXXXXXXD9E86A00");
  auto addr2 = GetMemoryAddressFromPattern("740DD944240851");
  if (addr1 != 0 && addr2 != 0) {
    spdlog::info("{}", "Skipping logos");
    writeToAddress(replace1, (void*)addr1, sizeof(replace1));
    writeToAddress(replace2, (void*)addr2, sizeof(replace2));
  }
}
