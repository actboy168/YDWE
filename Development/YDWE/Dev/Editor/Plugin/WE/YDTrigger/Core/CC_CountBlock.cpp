#include "CC_Include.h"

int _fastcall
CC_CountBlock(DWORD This, DWORD index)
{
  DWORD nItemCount, i;
  DWORD nItemClass;
  DWORD sum = 0;

  nItemCount = *(DWORD*)(This+0xC);
  
  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    if (*(DWORD*)(nItemClass+0x13C) != 0)
    {
      if ((index) == -1 || (*(DWORD*)(nItemClass+0x154) == index))
      {
        sum++;
      }
    }
  }

  return sum;
}
