/*
Pattern search algorithm and other memory related issues.
- thohell@home.se
*/
#include "memory.h"
#include <Psapi.h>
#include <cstdlib>
#include <cstring>

BOOL PatternEquals(LPBYTE buf, LPWORD pat, DWORD plen);
LPVOID PatternSearch(LPBYTE buf, DWORD blen, LPWORD pat, DWORD plen);
VOID MakeSearchPattern(LPCSTR pString, LPWORD pat);

/*
GetMemoryAddressFromPattern
_____________________________
Returns the address of szSearchPattern+offset if found in szDllName
The search criteria are determined by the first character of
szSearchPattern:
!:An ordinal inside	(e.g. !NameOfFuntion or !10005)
#:An actual hexadecimal address (e.g. #6fba80b4)
other:Fingerprint pattern (see below)
Patterns are interpreted as a string of bytes. The value 00 to ff
represents an actual value. A byte represented as 'xx' is not
important to the fingerprint and are masked out. Example of masked
bytes are absolute addresses/offsets inside the code that are
likely to change location on blizzard patches.
Once the address of the ordinal, actual address or fingerprint has
been found, the offset is added to the result and passed back to
the calling function.
If the address is not found the function returns 0
- thohell
*/
DWORD GetMemoryAddressFromPattern(LPCWSTR szDllName, LPCSTR szSearchPattern, DWORD offset) {
  DWORD lResult = 0;
  // Check for actual address
  if (szSearchPattern[0] == '#') {
    lResult = std::strtoul(&szSearchPattern[1], nullptr, 16);
    return lResult += (lResult ? offset : 0);
  }
  // Check for ordinal
  if (szSearchPattern[0] == '!') {
    HMODULE hModule = GetModuleHandleW(szDllName);
    // First let's try to find ordinal by name
    if (hModule) {
      lResult = (DWORD)GetProcAddress(hModule, &szSearchPattern[1]);
      // No luck, lets try by ordinal number instead
      if (!lResult) {
        lResult = (DWORD)GetProcAddress(
            hModule, (LPCSTR)MAKELONG(std::strtoul(&szSearchPattern[1], nullptr, 10), 0));
      }
    }
    return lResult += (lResult ? offset : 0);
  }
  // Parse fingerprint
  DWORD len = (strlen(szSearchPattern)) / 2;
  WORD* pPattern = new WORD[len];
  DWORD SearchSize = 0;
  DWORD SearchAddress = 0;
  MODULEINFO moduleInfo;
  HMODULE hDllModule = GetModuleHandleW(szDllName);
  if (hDllModule != nullptr) {
    if (GetModuleInformation(GetCurrentProcess(), hDllModule, &moduleInfo, sizeof(moduleInfo))) {
      SearchAddress = (DWORD)moduleInfo.lpBaseOfDll;
      SearchSize = moduleInfo.SizeOfImage;
      MakeSearchPattern(szSearchPattern, pPattern);
      if ((lResult = (DWORD)PatternSearch((BYTE*)SearchAddress, SearchSize, pPattern, len)))
        lResult += offset;
    } else {
      lResult = 0;
    }
  }
  delete[] pPattern;
  return lResult;
}

/*
Pattern search algorithm written by Druttis.
Patterns string is in the form of
0xMMVV, 0xMMVV, 0xMMVV
Where MM = Mask & VV = Value
Pattern Equals is doing the following match
(BB[p] & MM[p]) == VV[p]
Where BB = buffer data
That means:
a0, b0, c0, d0, e0 is equal to
1)	0xffa0, 0xffb0, 0x0000, 0x0000, 0xffe0
2)	0x0000, 0x0000, 0x0000, 0x0000, 0x0000
3)	0x8080, 0x3030, 0x0000, 0xffdd, 0xffee
I think you got the idea of it...BOOL _fastcall PatternEquals(LPBYTE buf, LPWORD pat, DWORD plen)
*/
BOOL PatternEquals(LPBYTE buf, LPWORD pat, DWORD plen) {
  // Offset
  DWORD ofs = 0;
  // Loop
  for (DWORD i = 0; plen > 0; i++) {
    // Compare mask buf and compare result.
    // Swapped mask/data. Old code was buggy.
    // - thohell
    if ((buf[ofs] & HIBYTE(pat[ofs]) /* mask */) != LOBYTE(pat[ofs]) /* value */)
      return FALSE;
    // Move ofs in zigzag direction
    plen--;
    if ((i & 1) == 0)
      ofs += plen;
    else
      ofs -= plen;
  }
  // Yep, we found
  return TRUE;
}

/*
Search for the pattern, returns the pointer to buf+ofset matching
the pattern or null.
*/
LPVOID PatternSearch(LPBYTE buf, DWORD blen, LPWORD pat, DWORD plen) {
  // Buffer length and Pattern length may not be 0
  if ((blen == 0) || (plen == 0))
    return nullptr;
  // Calculate End of search
  DWORD end = blen - plen;
  // Do the booring loop  
  for (DWORD ofs = 0; ofs != end; ofs++) { // Offset and End of search
    // Return offset to first byte of buf matching width the pattern
    if (PatternEquals(&buf[ofs], pat, plen))
      return &buf[ofs];
  }
  // Me no find, me return 0, nullptr, nil
  return nullptr;
}

/*
MakeSearchPattern
___________________
Convert a pattern-string into a pattern array for use with pattern
search.
- thohell
*/
VOID MakeSearchPattern(LPCSTR pString, LPWORD pat) {
  size_t len = std::strlen(pString) / 2;
  char tmp[3] = { };
  for (size_t i = 0; i < len; i++) {
    std::memcpy(tmp, &pString[i * 2], 2);
    char* x;
    BYTE value = (BYTE)std::strtoul(tmp, &x, 16);
    if (*x == '\0') // success
      pat[i] = MAKEWORD(value, 0xff /* mask */);
    else // failure
      pat[i] = 0;
  }
}
