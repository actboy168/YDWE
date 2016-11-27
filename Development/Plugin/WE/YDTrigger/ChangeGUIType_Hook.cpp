#include <windows.h>
#include "Storm/StormAdapter.h"
#include "Common.h"
#include "Core/CC_GUIID.h"

VOID ChangeGUIType_Set(DWORD This, DWORD EDX, DWORD unknow, int from, int to)
{
	const char* type = (const char*)&GetGUIVar_Value(This, from);
	if (type) {
		const char* pos = strchr(type, '_');
		if (pos) {
			SetGUIType(This, EDX, to, pos+1, unknow);
		}
	}
}

void _fastcall 
	ChangeGUIType_Hook(DWORD This, DWORD EDX, DWORD unknow)
{
	switch (*(DWORD*)(This+0x138))
	{
	case CC_GUIID_YDWESetAnyTypeLocalVariable:
		ChangeGUIType_Set(This, EDX, unknow, 0, 2);
		break;
	case CC_GUIID_YDWESetAnyTypeLocalArray:
		ChangeGUIType_Set(This, EDX, unknow, 0, 3);
		break;
	case CC_GUIID_YDWESaveAnyTypeDataByUserData:
		ChangeGUIType_Set(This, EDX, unknow, 0, 1);
		ChangeGUIType_Set(This, EDX, unknow, 3, 4);
		break;
	case CC_GUIID_YDWELoadAnyTypeDataByUserData:
	case CC_GUIID_YDWEHaveSavedAnyTypeDataByUserData:
	case CC_GUIID_YDWEFlushAnyTypeDataByUserData:
	case CC_GUIID_YDWEFlushAllByUserData:
		ChangeGUIType_Set(This, EDX, unknow, 0, 1);
		break;
	case CC_GUIID_YDWEGetObjectPropertyInteger:
	case CC_GUIID_YDWEGetObjectPropertyReal:
	case CC_GUIID_YDWEGetObjectPropertyString:
		ChangeGUIType_Set(This, EDX, unknow, 0, 1);
		break;
	default:
		break;
	}

	ChangeGUIType(This, EDX, unknow);
}