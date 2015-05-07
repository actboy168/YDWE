#include <windows.h>
#include "Storm/StormAdapter.h"
#include "Common.h"
#include "Core/CC_GUIID.h"

VOID ChangeGUIType_Set(DWORD This, DWORD EDX, DWORD unknow, int from, int to, size_t offset)
{
	char* type = (char*)&GetGUIVar_Value(This, from);
	if ((type != NULL) && (BLZSStrLen(type) > (int)offset))
		SetGUIType(This, EDX, to, type+offset, unknow);
}

void _fastcall 
	ChangeGUIType_Hook(DWORD This, DWORD EDX, DWORD unknow)
{
	switch (*(DWORD*)(This+0x138))
	{
	case CC_GUIID_YDWESetAnyTypeLocalVariable:
		ChangeGUIType_Set(This, EDX, unknow, 0, 2, 11);
		break;
	case CC_GUIID_YDWESaveAnyTypeDataByUserData:
		ChangeGUIType_Set(This, EDX, unknow, 0, 1, 11);
		ChangeGUIType_Set(This, EDX, unknow, 3, 4, 11);
		break;
	case CC_GUIID_YDWELoadAnyTypeDataByUserData:
	case CC_GUIID_YDWEHaveSavedAnyTypeDataByUserData:
	case CC_GUIID_YDWEFlushAnyTypeDataByUserData:
	case CC_GUIID_YDWEFlushAllByUserData:
		ChangeGUIType_Set(This, EDX, unknow, 0, 1, 11);
		break;
	case CC_GUIID_YDWEGetObjectPropertyInteger:
	case CC_GUIID_YDWEGetObjectPropertyReal:
	case CC_GUIID_YDWEGetObjectPropertyString:
		ChangeGUIType_Set(This, EDX, unknow, 0, 1, 13);
		break;
	default:
		break;
	}

	ChangeGUIType(This, EDX, unknow);
}