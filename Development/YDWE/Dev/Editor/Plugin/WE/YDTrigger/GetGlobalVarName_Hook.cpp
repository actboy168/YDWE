#include <windows.h>
#include "Storm/StormAdapter.h"
#include "ConvertString/ConvertString.h"

void _fastcall 
GetGlobalVarName_Hook(DWORD This, DWORD EDX, DWORD index, char* dst, int len)
{
  DWORD nClass = *(DWORD*)(This+0x0C)+index*0x1C0;
  char buff[260];
  
  ConvertString((char*)(nClass+0x2E), buff, 260);

  if (0 == *(DWORD*)(nClass))
  {
    BLZSStrCopy(dst, buff, len);
  }
  else
  {
    BLZSStrPrintf(dst, len, "%s%s", "udg_", buff);
  }
}