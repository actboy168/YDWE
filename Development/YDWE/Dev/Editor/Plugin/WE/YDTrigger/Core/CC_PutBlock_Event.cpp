#include "CC_Include.h"

void _fastcall CC_PutEventRegisterEx(DWORD This, DWORD OutClass, char* triggername, char* name, DWORD index);

void _fastcall
	CC_PutBlock_Event(DWORD This, DWORD OutClass, char* name, DWORD index, char* triggername)
{
	DWORD nItemCount, i;
	DWORD nItemClass;

	nItemCount = *(DWORD*)(This+0xC);

	for (i = 0; i < nItemCount; i++)
	{
		nItemClass = ((DWORD*)(*(DWORD*)(This+0x10)))[i];
		if ((index) == -1 || (*(DWORD*)(nItemClass+0x154) == index))
		{
			CC_PutEventRegisterEx(nItemClass, OutClass, triggername, name, i);
		}
	}
}
