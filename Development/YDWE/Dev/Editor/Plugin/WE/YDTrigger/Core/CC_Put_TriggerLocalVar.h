#ifndef INCLUDE_CC_PUT_TRIGGERLOCALVAR_H
#define INCLUDE_CC_PUT_TRIGGERLOCALVAR_H

void _fastcall CC_Put_TriggerLocalVar_Begin(DWORD OutClass);
void _fastcall CC_Put_TriggerLocalVar_End(DWORD OutClass);

void _fastcall CC_Put_TriggerLocalVar_Get(DWORD This, DWORD OutClass, char* name);
void _fastcall CC_Put_TriggerLocalVar_Set(DWORD This, DWORD OutClass, char* type_name);

void _fastcall CC_Put_TriggerLocalVar_Sleep_End(DWORD OutClass);

#endif