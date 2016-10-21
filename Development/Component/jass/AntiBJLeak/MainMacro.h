#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK

#ifndef INCLUDE_YDWE_ANTI_BJ_LEAK_H
#define INCLUDE_YDWE_ANTI_BJ_LEAK_H

#ifdef SCRIPT_INJECTION

#define PolledWait(a)                             YDWEPolledWaitNull(a)
#define PlaySound(a)                              YDWEPlaySoundNull(a)
#define GetCurrentCameraSetup()                   YDWEGetCurrentCameraSetupNull()
#define TriggerRegisterEnterRectSimple(a,b)       YDWETriggerRegisterEnterRectSimpleNull(a,b)
#define TriggerRegisterLeaveRectSimple(a,b)       YDWETriggerRegisterLeaveRectSimpleNull(a,b)
#define IssueHauntOrderAtLocBJ(a,b)               YDWEIssueHauntOrderAtLocBJNull(a,b)
#define IssueBuildOrderByIdLocBJ(a,b,c)           YDWEIssueBuildOrderByIdLocBJNull(a,b,c)
#define WakePlayerUnits(a)                        YDWEWakePlayerUnitsNull(a)
#define EnableCreepSleepBJ(a)                     YDWEEnableCreepSleepBJNull(a)
#define PauseAllUnitsBJ(a)                        YDWEPauseAllUnitsBJNull(a)
#define ReplaceUnitBJ(a,b,c)                      YDWEReplaceUnitBJNull(a,b,c)
#define EnumDestructablesInCircleBJFilter()       YDWEEnumDestructablesInCircleBJFilterNull()
#define EnumDestructablesInCircleBJ(a,b,c)        YDWEEnumDestructablesInCircleBJNull(a,b,c)
#define NudgeUnitsInRectEnum()                    YDWENudgeUnitsInRectEnumNull()
#define NudgeItemsInRectEnum()                    YDWENudgeItemsInRectEnumNull()
#define NudgeObjectsInRect(a)                     YDWENudgeObjectsInRectNull(a)
#define NearbyElevatorExistsEnum()                YDWENearbyElevatorExistsEnumNull()
#define NearbyElevatorExists(a,b)                 YDWENearbyElevatorExistsNull(a,b)
#define EnumUnitsSelected(a,b,c)                  YDWEEnumUnitsSelectedNull(a,b,c)
#define ChangeElevatorWallBlocker(a,b,c,d)        YDWEChangeElevatorWallBlockerNull(a,b,c,d)
#define ChangeElevatorWalls(a,b,c)                YDWEChangeElevatorWallsNull(a,b,c)
#define GetUnitsInRectMatching(a,b)               YDWEGetUnitsInRectMatchingNull(a,b)
#define GetUnitsInRectAll(a)                      YDWEGetUnitsInRectAllNull(a)
#define GetUnitsInRectOfPlayer(a,b)               YDWEGetUnitsInRectOfPlayerNull(a,b)
#define GetUnitsInRangeOfLocMatching(a,b,c)       YDWEGetUnitsInRangeOfLocMatchingNull(a,b,c)
#define GetUnitsInRangeOfLocAll(a,b)              YDWEGetUnitsInRangeOfLocAllNull(a,b)
#define GetUnitsOfTypeIdAll(a)                    YDWEGetUnitsOfTypeIdAllNull(a)
#define GetUnitsOfPlayerMatching(a,b)             YDWEGetUnitsOfPlayerMatchingNull(a,b)
#define GetUnitsOfPlayerAll(a)                    YDWEGetUnitsOfPlayerAllNull(a)
#define GetUnitsOfPlayerAndTypeId(a,b)            YDWEGetUnitsOfPlayerAndTypeIdNull(a,b)
#define GetUnitsSelectedAll(a)                    YDWEGetUnitsSelectedAllNull(a)
#define GetForceOfPlayer(a)                       YDWEGetForceOfPlayerNull(a)
#define GetPlayersByMapControl(a)                 YDWEGetPlayersByMapControlNull(a)
#define GetPlayersAllies(a)                       YDWEGetPlayersAlliesNull(a)
#define GetPlayersEnemies(a)                      YDWEGetPlayersEnemiesNull(a)
#define GetPlayersMatching(a)                     YDWEGetPlayersMatchingNull(a)
#define GetRandomSubGroupEnum()                   YDWEGetRandomSubGroupEnumNull()
#define GetRandomSubGroup(a,b)                    YDWEGetRandomSubGroupNull(a,b)
#define CountLivingPlayerUnitsOfTypeId(a,b)       YDWECountLivingPlayerUnitsOfTypeIdNull(a,b)
#define SetUnitFacingToFaceLocTimed(a,b,c)        YDWESetUnitFacingToFaceLocTimedNull(a,b,c)
#define SetUnitFacingToFaceUnitTimed(a,b,c)       YDWESetUnitFacingToFaceUnitTimedNull(a,b,c)
#define MakeUnitsPassiveForPlayer(a)              YDWEMakeUnitsPassiveForPlayerNull(a)
#define MakeUnitsPassiveForTeam(a)                YDWEMakeUnitsPassiveForTeamNull(a)
#define MultiboardSetItemStyleBJ(a,b,c,d,e)       YDWEMultiboardSetItemStyleBJNull(a,b,c,d,e)
#define MultiboardSetItemValueBJ(a,b,c,d)         YDWEMultiboardSetItemValueBJNull(a,b,c,d)
#define MultiboardSetItemColorBJ(a,b,c,d,e,f,g)   YDWEMultiboardSetItemColorBJNull(a,b,c,d,e,f,g)
#define MultiboardSetItemWidthBJ(a,b,c,d)         YDWEMultiboardSetItemWidthBJNull(a,b,c,d)
#define MultiboardSetItemIconBJ(a,b,c,d)          YDWEMultiboardSetItemIconBJNull(a,b,c,d)
#define TriggerActionUnitRescuedBJ()              YDWETriggerActionUnitRescuedBJNull()
#define TryInitRescuableTriggersBJ()              YDWETryInitRescuableTriggersBJNull()
#define MakeUnitRescuableToForceBJEnum()          YDWEMakeUnitRescuableToForceBJEnumNull()
#define MakeUnitRescuableToForceBJ(a,b,c)         YDWEMakeUnitRescuableToForceBJNull(a,b,c)
#define BlightGoldMineForPlayerBJ(a,b)            YDWEBlightGoldMineForPlayerBJNull(a,b)
#define BlightGoldMineForPlayer(a,b)              YDWEBlightGoldMineForPlayerNull(a,b)
#define SetPlayerColorBJ(a,b,c)                   YDWESetPlayerColorBJNull(a,b,c)
#define InitRescuableBehaviorBJ()                 YDWEInitRescuableBehaviorBJNull()
#define DelayedSuspendDecayStopAnimEnum()         YDWEDelayedSuspendDecayStopAnimEnumNull()
#define DelayedSuspendDecayFleshEnum()            YDWEDelayedSuspendDecayFleshEnumNull()
#define DelayedSuspendDecayBoneEnum()             YDWEDelayedSuspendDecayBoneEnumNull()
#define DelayedSuspendDecay()                     YDWEDelayedSuspendDecayNull()
#define SmartCameraPanBJ(a,b,c)                   YDWESmartCameraPanBJNull(a,b,c)
#define GetInventoryIndexOfItemTypeBJ(a,b)        YDWEGetInventoryIndexOfItemTypeBJNull(a,b)
#define GetItemOfTypeFromUnitBJ(a,b)              YDWEGetItemOfTypeFromUnitBJNull(a,b)
#define UnitHasItemOfTypeBJ(a,b)                  YDWEUnitHasItemOfTypeBJNull(a,b)

#endif
#endif
#endif
