#ifndef INCLUDE_CC_SP_USERDATA_H
#define INCLUDE_CC_SP_USERDATA_H

VOID _fastcall CC_Put_YDWESaveAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name);
VOID _fastcall CC_Put_YDWELoadAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name, char* type_name);
VOID _fastcall CC_Put_YDWEHaveSavedAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name);
VOID _fastcall CC_Put_YDWEFlushAnyTypeDataByUserData(DWORD This, DWORD OutClass, char* name);
VOID _fastcall CC_Put_YDWEFlushAllByUserData(DWORD This, DWORD OutClass, char* name);

#endif
