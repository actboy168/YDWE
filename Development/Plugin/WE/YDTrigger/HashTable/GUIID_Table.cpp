#include "hashtable.h"
#include "../Core/CC_GUIID.h"

#define GUIID_Table_Put(NAME) (GUIID_Table_PutID(#NAME, CC_GUIID_##NAME))

hashtable*   g_guid_table;

void
GUIID_Table_Create()
{
  g_guid_table = hashtable_new(0x1000);
}

void
GUIID_Table_Destory()
{
  hashtable_delete(g_guid_table);
}

void
GUIID_Table_PutID(const char* gui_name, unsigned int id)
{
  hashtable_put(g_guid_table, gui_name, id);
}

void
GUIID_Table_GetID(const char* gui_name, unsigned int* pid)
{
  hashtable_get(g_guid_table, gui_name, pid);
}

void
GUIID_Table_Init()
{
  //---------------------------------------------
  //  WE_Defined_ID
  //---------------------------------------------
  GUIID_Table_Put(SetVariable);
  GUIID_Table_Put(GetBooleanVariableValue);
  GUIID_Table_Put(OperatorString);
  GUIID_Table_Put(IfThenElse);
  GUIID_Table_Put(IfThenElseMultiple);
  GUIID_Table_Put(ForLoopA);
  GUIID_Table_Put(ForLoopB);
  GUIID_Table_Put(ForLoopVar);
  GUIID_Table_Put(ForLoopAMultiple);
  GUIID_Table_Put(ForLoopBMultiple);
  GUIID_Table_Put(ForLoopVarMultiple);
  GUIID_Table_Put(ReturnAction);
  GUIID_Table_Put(RunTriggerActions);
  GUIID_Table_Put(TestTriggerConditions);
  GUIID_Table_Put(GetBooleanAnd);
  GUIID_Table_Put(GetBooleanOr);
  GUIID_Table_Put(AndMultiple);
  GUIID_Table_Put(OrMultiple);
  GUIID_Table_Put(WaitForCondition);
  GUIID_Table_Put(CommentString);
  GUIID_Table_Put(CustomScriptCode);
  GUIID_Table_Put(AddTriggerEvent);
  GUIID_Table_Put(ForGroupMultiple);
  GUIID_Table_Put(ForForceMultiple);
  GUIID_Table_Put(EnumDestructablesInRectAllMultiple);
  GUIID_Table_Put(EnumDestructablesInCircleBJMultiple);
  GUIID_Table_Put(EnumItemsInRectBJMultiple);
  GUIID_Table_Put(GetTriggerName);

  //---------------------------------------------
  // User_Define_ID
  //---------------------------------------------
  GUIID_Table_Put(YDWESetAnyTypeLocalVariable);
  GUIID_Table_Put(YDWEGetAnyTypeLocalVariable);
  GUIID_Table_Put(YDWESaveAnyTypeDataByUserData);
  GUIID_Table_Put(YDWELoadAnyTypeDataByUserData);
  GUIID_Table_Put(YDWEHaveSavedAnyTypeDataByUserData);
  GUIID_Table_Put(YDWEFlushAnyTypeDataByUserData);
  GUIID_Table_Put(YDWEFlushAllByUserData);
  GUIID_Table_Put(YDWETimerStartMultiple);
  GUIID_Table_Put(YDWETimerStartFlush);
  GUIID_Table_Put(YDWERegisterTriggerMultiple);
  GUIID_Table_Put(YDWERegisterTriggerFlush);
  GUIID_Table_Put(YDWEForLoopLocVarMultiple);
  GUIID_Table_Put(YDWEForLoopLocVarIndex);
  GUIID_Table_Put(YDWECustomScriptCode);
  GUIID_Table_Put(YDWEEnumUnitsInRangeMultiple);
  GUIID_Table_Put(YDWERegionMultiple);

  GUIID_Table_Put(YDWEActivateTrigger);
  GUIID_Table_Put(GetEnumUnit);
  GUIID_Table_Put(GetFilterUnit);
  GUIID_Table_Put(TriggerSleepAction);
  GUIID_Table_Put(PolledWait);
  GUIID_Table_Put(YDWEGetObjectPropertyInteger);
  GUIID_Table_Put(YDWEGetObjectPropertyReal);
  GUIID_Table_Put(YDWEGetObjectPropertyString);
}
