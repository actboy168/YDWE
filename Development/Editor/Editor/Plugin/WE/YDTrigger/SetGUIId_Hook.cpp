#include <windows.h>
#include "Storm/StormAdapter.h"
#include "Common.h"
#include "HashTable/GUIID_Table.h"

int _fastcall
SetGUIId_Hook(DWORD This, DWORD EDX, char* name)
{
  BLZSStrCopy((char*)(This+0x20), name, 0xFFFFFFFF);
  GUIID_Table_GetID((char*)(This+0x20), (unsigned int*)(This+0x138));
  return SetGUIUnknow(This); 
}
