/*
Pattern search algorithm and other memory related issues.
- thohell@home.se
*/
#include "memory.h"

#include <Psapi.h>
#include <stdexcept>
#include <vector>

bool PatternEquals(PBYTE buf, const std::vector<WORD>& pat);
PBYTE PatternSearch(PBYTE buf, DWORD blen, const std::vector<WORD>& pat);
void MakeSearchPattern(const std::string& str, std::vector<WORD>& pat);

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
PBYTE GetMemoryAddressFromPattern(const std::string& searchPattern) {
  // Parse fingerprint
  MODULEINFO moduleInfo;
  if (GetModuleInformation(GetCurrentProcess(), GetModuleHandleW(nullptr), &moduleInfo,
                           sizeof(moduleInfo))) {
    auto SearchAddress = reinterpret_cast<PBYTE>(moduleInfo.lpBaseOfDll);
    auto SearchSize = moduleInfo.SizeOfImage;
    std::vector<WORD> pattern;
    pattern.resize(searchPattern.size() / 2);
    MakeSearchPattern(searchPattern, pattern);
    return PatternSearch(SearchAddress, SearchSize, pattern);
  }
  return 0;
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
I think you got the idea of it...BOOL _fastcall PatternEquals(PBYTE buf, LPWORD pat, DWORD plen)
*/
bool PatternEquals(PBYTE buf, const std::vector<WORD>& pat) {
  auto plen = pat.size();
  // Offset
  DWORD ofs = 0;
  // Loop
  for (DWORD i = 0; plen > 0; i++) {
    // Compare mask buf and compare result.
    // Swapped mask/data. Old code was buggy.
    // - thohell
    if ((buf[ofs] & HIBYTE(pat[ofs]) /* mask */) != LOBYTE(pat[ofs]) /* value */)
      return false;
    // Move ofs in zigzag direction
    plen--;
    if ((i & 1) == 0)
      ofs += plen;
    else
      ofs -= plen;
  }
  // Yep, we found
  return true;
}

/*
Search for the pattern, returns the pointer to buf+ofset matching
the pattern or null.
*/
PBYTE PatternSearch(PBYTE buf, DWORD blen, const std::vector<WORD>& pat) {
  // Buffer length and Pattern length may not be 0
  if ((blen == 0) || pat.empty())
    return nullptr;
  // Calculate End of search
  DWORD end = blen - pat.size();
  // Do the booring loop
  for (DWORD ofs = 0; ofs != end; ofs++) { // Offset and End of search
    // Return offset to first byte of buf matching width the pattern
    if (PatternEquals(&buf[ofs], pat))
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
void MakeSearchPattern(const std::string& str, std::vector<WORD>& pat) {
  for (size_t i = 0; i < pat.size(); i++) {
    std::string tmp = str.substr(i * 2, 2);
    try {
      BYTE value = static_cast<BYTE>(std::stoul(tmp, nullptr, 16));
      pat[i] = MAKEWORD(value, 0xff /* mask */);
    } catch (const std::invalid_argument&) {
      pat[i] = 0;
    }
  }
}

void writeToAddress(const void* Data, void* Address, size_t Size) {
  DWORD oldProtect;
  if (::VirtualProtect(Address, Size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
    ::CopyMemory(Address, Data, Size);
    ::VirtualProtect(Address, Size, oldProtect, &oldProtect);
  }
}
