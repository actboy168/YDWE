#include "CC_Include.h"

void _fastcall 
CC_Put_endglobals_Hook(DWORD OutClass)
{
  ((DWORD*)OutClass)[3] = ((DWORD*)OutClass)[3] > 1?  ((DWORD*)OutClass)[3]-1: 0;
  
  CC_PutString(OutClass, 0, "#include <YDTrigger/Globals.h>", 1);
  CC_PutString(OutClass, 0, "endglobals", 1);
  CC_PutString(OutClass, 0, "#include <YDTrigger/Function.h>", 1);
}
