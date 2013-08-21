#include "CC_Include.h"

void _fastcall
CC_PutTrigger_Condition_And(DWORD This, DWORD OutClass, char* name)
{
  DWORD nItemCount, i;
  DWORD nItemClass;
  DWORD nofirst = FALSE;

  nItemCount = *(DWORD*)(This+0xC);
  
  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    if (Proc_GetGUIType(nItemClass) == CC_GUI_TYPE_CONDITION)
    {
      if (*(DWORD*)(nItemClass+0x13C) != 0)
      {
        if (nofirst)
        {
          PUT_CONST(" and ", 0);
        }
        else
        {
          nofirst = TRUE;
        }

        CC_PutCondition(nItemClass, OutClass, name, i);
      }
    }
  }

  if (!nofirst)
  {
    PUT_CONST("true", 0);
  }
}

void _fastcall
CC_PutBlock_Condition_Or(DWORD This, DWORD OutClass, char* name, DWORD index)
{
  DWORD nItemCount, i;
  DWORD nItemClass;
  DWORD nofirst = FALSE;

  nItemCount = *(DWORD*)(This+0xC);
  
  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    if (*(DWORD*)(nItemClass+0x13C) != 0)
    {
      if ((index) == -1 || (*(DWORD*)(nItemClass+0x154) == index))
      {
        if (nofirst)
        {
          PUT_CONST(" or ", 0);
        }
        else
        {
          nofirst = TRUE;
        }

        CC_PutCondition(nItemClass, OutClass, name, i);
      }
    }
  }

  if (!nofirst)
  {
    PUT_CONST("false", 0);
  }
}

void _fastcall
CC_PutBlock_Condition_And(DWORD This, DWORD OutClass, char* name, DWORD index)
{
  DWORD nItemCount, i;
  DWORD nItemClass;
  DWORD nofirst = FALSE;

  nItemCount = *(DWORD*)(This+0xC);
  
  for (i = 0; i < nItemCount; i++)
  {
    nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
    if (*(DWORD*)(nItemClass+0x13C) != 0)
    {
      if ((index) == -1 || (*(DWORD*)(nItemClass+0x154) == index))
      {
        if (nofirst)
        {
          PUT_CONST(" and ", 0);
        }
        else
        {
          nofirst = TRUE;
        }

        CC_PutCondition(nItemClass, OutClass, name, i);
      }
    }
  }

  if (!nofirst)
  {
    PUT_CONST("true", 0);
  }
}
