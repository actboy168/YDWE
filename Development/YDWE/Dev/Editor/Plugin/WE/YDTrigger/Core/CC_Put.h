#ifndef INCLUDE_CC_PUT_H
#define INCLUDE_CC_PUT_H

#define PUT_CONST(String, Endl) CC_PutConst(OutClass, (String), (Endl))
#define PUT_VAR(This, Index)   CC_PutVar((This), 0, OutClass, name, (Index), 2, 0)

void _fastcall CC_PutLocal_Begin(DWORD This, DWORD OutClass, DWORD isTimer, DWORD isSearchHashLocal);
void _fastcall CC_PutLocal_End(DWORD This, DWORD OutClass, DWORD isTimer, DWORD isEnd);
void _fastcall CC_PutAction(DWORD This, DWORD OutClass, char* name, DWORD index, DWORD Endl);
void _fastcall CC_PutConditionEx(DWORD This, DWORD OutClass, char* name);
void _fastcall CC_PutCondition(DWORD This, DWORD OutClass, char* name, DWORD index);
int  _fastcall CC_CountBlock(DWORD This, DWORD index);
void _fastcall CC_PutVar_Code(DWORD This, DWORD OutClass, char* name, DWORD index, DWORD Type);
void _fastcall CC_PutExternFunc(DWORD This, DWORD OutClass, char* name, DWORD index);
void _fastcall CC_PutBlock_Action(DWORD This, DWORD OutClass, char* name, LONG index);
void _fastcall CC_PutBlock_Event(DWORD This, DWORD OutClass, char* name, DWORD index, char* triggername);
void _fastcall CC_PutBlock_Condition_And(DWORD This, DWORD OutClass, char* name, DWORD index);
void _fastcall CC_PutBlock_Condition_Or(DWORD This, DWORD OutClass, char* name, DWORD index);
void _fastcall CC_PutTrigger_Condition_And(DWORD This, DWORD OutClass, char* name);

void _fastcall GetTriggerName_Trig(DWORD This, char* buff, int len);

void ShowError(DWORD OutClass, LPCSTR szErrorText);
void ShowErrorN(DWORD OutClass, LPCSTR szErrorText, ...);

void   SaveLoadCheck_Reset();
BOOL   SaveLoadCheck_Set(LPCSTR lpszKey, LPCSTR lpszName);
LPCSTR SaveLoadCheck_Get(LPCSTR lpszKey);

#endif