#include "CC_Include.h"

void _fastcall
CC_PutBlock_Action(DWORD This, DWORD OutClass, char* name, LONG index)
{
  DWORD nItemCount, i;
  DWORD nItemClass;

  nItemCount = *(DWORD*)(This+0xC);

  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    if (*(DWORD*)(nItemClass+0x13C) != 0)
    {
      if ((index) == -1 || (*(LONG*)(nItemClass+0x154) == index))
      {
        CC_PutAction(nItemClass, OutClass, name, i, 0);
      }
    }
  }
}
