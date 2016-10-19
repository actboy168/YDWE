#ifndef INCLUDE_CC_GUIID_USERDEFINE_H
#define INCLUDE_CC_GUIID_USERDEFINE_H

#define CC_GUIID_USER_DEFINE 0x100

enum GUIID_User_Defined
{
  CC_GUIID_YDWESetAnyTypeLocalVariable = CC_GUIID_USER_DEFINE,
  CC_GUIID_YDWEGetAnyTypeLocalVariable,
  CC_GUIID_YDWESaveAnyTypeDataByUserData,
  CC_GUIID_YDWELoadAnyTypeDataByUserData,
  CC_GUIID_YDWEHaveSavedAnyTypeDataByUserData,
  CC_GUIID_YDWEFlushAnyTypeDataByUserData,
  CC_GUIID_YDWEFlushAllByUserData,
  CC_GUIID_YDWETimerStartMultiple,
  CC_GUIID_YDWETimerStartFlush,
  CC_GUIID_YDWERegisterTriggerMultiple,
  CC_GUIID_YDWERegisterTriggerFlush,
  CC_GUIID_YDWEForLoopLocVarMultiple,
  CC_GUIID_YDWEForLoopLocVarIndex,
  CC_GUIID_YDWECustomScriptCode,
  CC_GUIID_YDWEEnumUnitsInRangeMultiple,
  CC_GUIID_YDWEActivateTrigger,
  CC_GUIID_GetEnumUnit,
  CC_GUIID_GetFilterUnit,
  CC_GUIID_TriggerSleepAction,
  CC_GUIID_PolledWait,
  CC_GUIID_YDWEGetObjectPropertyInteger,
  CC_GUIID_YDWEGetObjectPropertyReal,
  CC_GUIID_YDWEGetObjectPropertyString,
  CC_GUIID_YDWERegionMultiple,
};
//--------------------------------------------------------
//  WE_Defined_ID
//--------------------------------------------------------
enum GUIID_WE_Defined
{
  CC_GUIID_SetVariable = 0,
  CC_GUIID_GetBooleanVariableValue,
  CC_GUIID_OperatorString,
  CC_GUIID_IfThenElse,
  CC_GUIID_IfThenElseMultiple,
  CC_GUIID_ForLoopA,
  CC_GUIID_ForLoopB,
  CC_GUIID_ForLoopVar,
  CC_GUIID_ForLoopAMultiple,
  CC_GUIID_ForLoopBMultiple,
  CC_GUIID_ForLoopVarMultiple,
  CC_GUIID_ReturnAction,
  CC_GUIID_RunTriggerActions,
  CC_GUIID_TestTriggerConditions,
  CC_GUIID_GetBooleanAnd,
  CC_GUIID_GetBooleanOr,
  CC_GUIID_AndMultiple,
  CC_GUIID_OrMultiple,
  CC_GUIID_WaitForCondition,
  CC_GUIID_CommentString,
  CC_GUIID_CustomScriptCode,
  CC_GUIID_AddTriggerEvent,
  CC_GUIID_ForGroupMultiple,
  CC_GUIID_ForForceMultiple,
  CC_GUIID_EnumDestructablesInRectAllMultiple,
  CC_GUIID_EnumDestructablesInCircleBJMultiple,
  CC_GUIID_EnumItemsInRectBJMultiple,
  CC_GUIID_GetTriggerName,
};
//--------------------------------------------------------
//--------------------------------------------------------
#endif