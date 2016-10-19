#include <windows.h>
#include "Storm/StormAdapter.h"
#include "Common.h"
#include "Core/CC_GUIID.h"

int _fastcall
GetGUICount_Hook(DWORD This)
{
    switch (*(DWORD*)(This+0x138))
    {
    case CC_GUIID_YDWETimerStartMultiple:
        return 2;
    case CC_GUIID_YDWERegisterTriggerMultiple:
        return 3;
    case CC_GUIID_YDWEEnumUnitsInRangeMultiple:
    case CC_GUIID_YDWEForLoopLocVarMultiple:
	case CC_GUIID_YDWERegionMultiple:
        return 1;
    }

    return GetGUICount(This);
}

int _fastcall
GetGUIString_Hook(DWORD This, DWORD EDX, int index, char* buff, int len)
{
    switch (*(DWORD*)(This+0x138))
    {
    case CC_GUIID_YDWETimerStartMultiple:
        {
            if (index == 0)
                return GetWEString("WESTRING_PARAMETERS", buff, len, 0);
            else
                return GetWEString("WESTRING_ACTIONS", buff, len, 0);
        }
    case CC_GUIID_YDWERegisterTriggerMultiple:
		{
			if (index == 0)
				return GetWEString("WESTRING_EVENTS", buff, len, 0);
			else if (index == 1)
				return GetWEString("WESTRING_PARAMETERS", buff, len, 0);
            else
                return GetWEString("WESTRING_ACTIONS", buff, len, 0);
        }
    case CC_GUIID_YDWEEnumUnitsInRangeMultiple:
        {
            return GetWEString("WESTRING_ACTIONS", buff, len, 0);
        }
    case CC_GUIID_YDWEForLoopLocVarMultiple:
        {
            return GetWEString("WESTRING_TRIGSUBFUNC_FORLOOPACTIONS", buff, len, 0);
        }
	case CC_GUIID_YDWERegionMultiple: 
        {
            return GetWEString("WESTRING_ACTIONS", buff, len, 0);
        }
    }

    return GetGUIString(This, EDX, index, buff, len);
}

int _fastcall
GetGUIIcon_Hook(DWORD This, DWORD EDX, int index)
{
    switch (*(DWORD*)(This+0x138))
    {
    case CC_GUIID_YDWEEnumUnitsInRangeMultiple:
    case CC_GUIID_YDWETimerStartMultiple:
    case CC_GUIID_YDWEForLoopLocVarMultiple:
	case CC_GUIID_YDWERegionMultiple:
        {
            return CC_GUI_TYPE_ACTION;
        }
    case CC_GUIID_YDWERegisterTriggerMultiple:
		{
			if (index == 0)
				return CC_GUI_TYPE_EVENT;
			else if (index == 1)
                return CC_GUI_TYPE_ACTION;
            else
                return CC_GUI_TYPE_ACTION;
        }
    }

    return GetGUIIcon(This, EDX, index);
}
