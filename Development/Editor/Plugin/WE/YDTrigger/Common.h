#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H

#include <windows.h>

typedef int  (_fastcall* CC_VoidProc)(void);
typedef int  (_fastcall* CC_PutConstProc)(DWORD OutClass, const char* string, DWORD Endl);
typedef int  (_fastcall* CC_PutStringProc)(DWORD OutClass, DWORD EDX, char* string, DWORD Endl);
typedef int  (_fastcall* CC_PutVarProc)(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD Index, DWORD Type, DWORD Endl);
typedef int  (_fastcall* CC_PutEventRegisterProc)(DWORD This, DWORD EDX, DWORD OutClass, char* triggername, char* name, DWORD index);
typedef int  (_fastcall* CC_GetGUINameProc)(DWORD This, DWORD EDX, char* buff, int len);
typedef int  (_fastcall* GetTriggerVarProc)(DWORD This, DWORD EDX, char* buff, int len);
typedef int  (_fastcall* GetTriggerName_InitTrigProc)(DWORD This, DWORD EDX, char* buff, int len);
typedef int  (_fastcall* GetWEStringProc)(const char* Name, char* buff, int len, DWORD Unknow);
typedef int  (_fastcall* SetGUIUnknowProc)(DWORD This);
typedef void (_fastcall* CC_PutActionExProc)(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD Type, DWORD Endl);
typedef void (_fastcall* SetGUITypeProc)(DWORD This, DWORD EDX, DWORD index, char* name, DWORD unknow);
typedef void (_fastcall* CC_GetGlobalVarProc)(DWORD This, DWORD EDX, char* buff, DWORD len);

typedef void (_fastcall* CC_PutTriggerProc)(DWORD This, DWORD EDX, DWORD OutClass);
typedef void (_fastcall* CC_PutVar_OtherProc)(DWORD This, DWORD EDX, DWORD OutClass, char* name, DWORD index, DWORD type);
typedef int  (_fastcall* GetGUICountProc)(DWORD This);
typedef int  (_fastcall* GetGUIStringProc)(DWORD This, DWORD EDX, int index, char* buff, int len);
typedef int  (_fastcall* GetGUIIconProc)(DWORD This, DWORD EDX, int index);
typedef int  (_fastcall* SetGUIIdProc)(DWORD This, DWORD EDX, char* name);
typedef void (_fastcall* ConvertTriggerNameProc)(char* src, char* dst, int len);
typedef void (_fastcall* GetGlobalVarNameProc)(DWORD This, DWORD EDX, DWORD index, char* dst, int len);
typedef void (_fastcall* ChangeGUITypeProc)(DWORD This, DWORD EDX, DWORD unknow);
typedef void (_fastcall* CC_Put_globalsProc)(DWORD OutClass);
typedef void (_fastcall* CC_Put_endglobalsProc)(DWORD OutClass);
typedef void (_fastcall* CC_MainProc)(DWORD OutClass);

extern CC_VoidProc                 CC_PutBegin;
extern CC_VoidProc                 CC_PutEnd;
extern CC_PutConstProc             CC_PutConst;
extern CC_PutVarProc               CC_PutVar;
extern CC_PutEventRegisterProc     CC_PutEventRegister;
extern CC_GetGUINameProc           CC_GetGUIName;
extern GetTriggerVarProc           GetTriggerVar;
extern GetTriggerName_InitTrigProc GetTriggerName_InitTrig;
extern GetWEStringProc             GetWEString;
extern SetGUIUnknowProc            SetGUIUnknow;
extern CC_PutActionExProc          CC_PutActionEx;
extern SetGUITypeProc              SetGUIType;
extern CC_GetGlobalVarProc         CC_GetGlobalVar;
extern CC_PutStringProc            CC_PutString;

extern CC_PutTriggerProc           CC_PutTrigger;
extern CC_PutVar_OtherProc         CC_PutVar_Other;
extern GetGUICountProc             GetGUICount;
extern GetGUIStringProc            GetGUIString;
extern GetGUIIconProc              GetGUIIcon;
extern SetGUIIdProc                SetGUIId;
extern ConvertTriggerNameProc      ConvertTriggerName;
extern GetGlobalVarNameProc        GetGlobalVarName;
extern ChangeGUITypeProc           ChangeGUIType;
extern CC_Put_globalsProc          CC_Put_globals;
extern CC_Put_endglobalsProc       CC_Put_endglobals;
extern CC_MainProc                 CC_Main;

#define CC_GUI_TYPE_EVENT     0
#define CC_GUI_TYPE_CONDITION 1
#define CC_GUI_TYPE_ACTION    2

#define GetGUIVar_Value(This, index) (*(DWORD*)(((DWORD*)(*(DWORD*)((This)+0x12C)))[(index)]+0x4C))
#define GetGUIVar_Type(This, index)  (*(DWORD*)(((DWORD*)(*(DWORD*)((This)+0x12C)))[(index)]+0x48))
#define GetGUIVar_Class(This, index) (*(DWORD*)(((DWORD*)(*(DWORD*)((This)+0x12C)))[(index)]+0x178))

#define Proc_GetGUIType(This)  ((*(int (_fastcall **)(DWORD))(*(DWORD*)(This)+0x08))(This))
// =================================

void All_Hook();
void All_Unhook();

#define WE_ADDRESS(addr) ((DWORD)(addr)-0x00400000+g_nWEBase)
extern DWORD g_nWEBase;



#include <functional>
#include <cstdint>

inline void cc_search_var(uint32_t ptr, std::function<void(uint32_t)> func)
{
	uint32_t var_count = *(uint32_t*)(ptr+0x128);
	for (uint32_t i = 0; i < var_count; ++i)
	{
		uint32_t var_ptr = GetGUIVar_Class(ptr, i);
		if (var_ptr)
		{
			cc_search_var(var_ptr, func);
			func(var_ptr);
		}    
	}
}

#endif