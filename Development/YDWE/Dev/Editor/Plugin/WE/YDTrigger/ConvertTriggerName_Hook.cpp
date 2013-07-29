#include <windows.h>
#include "Storm/StormAdapter.h"
#include "ConvertString/ConvertString.h"
#include "Common.h"

void _fastcall 
ConvertTriggerName_Hook(char* src, char* dst, int len)
{
  ConvertString(src, dst, len);
}

void _fastcall 
GetTriggerName_Trig(DWORD This, char* dst, int len)
{
  char buff[260];

  if (*(DWORD*)(This+0x18))
  {
    dst[0] = 0;
  }
  else
  {
    ConvertTriggerName((char*)(This+0x4C), buff, 260);
    //ConvertString((char*)(This+0x4C), buff, 260);
    BLZSStrPrintf(dst, len, "%s%s", "Trig_", buff);
  }
}
