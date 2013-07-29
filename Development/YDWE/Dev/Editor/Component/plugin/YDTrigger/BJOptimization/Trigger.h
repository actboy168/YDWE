# /*
#  *  BJº¯ÊýÓÅ»¯ -- Trigger
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_TRIGGER_H
#define INCLUDE_BJ_OPTIMIZATION_TRIGGER_H
#
//#  include <YDTrigger/BJOptimization/detail/TriggerRegisterPlayerSelectionEventBJ.h>
//#  include <YDTrigger/BJOptimization/detail/TriggerRegisterPlayerKeyEventBJ.h>
#
#  define TriggerRegisterTimerEventPeriodic(trig, timeout)                 TriggerRegisterTimerEvent(trig, timeout, true)
#  define TriggerRegisterTimerEventSingle(trig, timeout)                   TriggerRegisterTimerEvent(trig, timeout, false)
#  define TriggerRegisterTimerExpireEventBJ(trig, t)                       TriggerRegisterTimerExpireEvent(trig, t)
//#  define TriggerRegisterPlayerUnitEventSimple(trig, p, e)                 TriggerRegisterPlayerUnitEvent(trig, p, e, null)
//#  define TriggerRegisterPlayerEventVictory(trig, player)                  TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_VICTORY)
//#  define TriggerRegisterPlayerEventDefeat(trig, player)                   TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_DEFEAT)
//#  define TriggerRegisterPlayerEventLeave(trig, player)                    TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_LEAVE)
//#  define TriggerRegisterPlayerEventAllianceChanged(trig, player)          TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ALLIANCE_CHANGED)
//#  define TriggerRegisterPlayerEventEndCinematic(trig, player)             TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_END_CINEMATIC)
#  define TriggerRegisterGameStateEventTimeOfDay(trig, opcode, limitval)   TriggerRegisterGameStateEvent(trig, GAME_STATE_TIME_OF_DAY, opcode, limitval)
#  define TriggerRegisterEnterRegionSimple(trig, region)                   TriggerRegisterEnterRegion(trig, region, null)
#  define TriggerRegisterLeaveRegionSimple(trig, region)                   TriggerRegisterLeaveRegion(trig, region, null)
#  define TriggerRegisterDistanceBetweenUnits(trig, unit, cond, range)     TriggerRegisterUnitInRange(trig, unit, range, cond)
#  define TriggerRegisterUnitInRangeSimple(trig, range, unit)              TriggerRegisterUnitInRange(trig, unit, range, null)
#  define TriggerRegisterUnitLifeEvent(trig, unit, opcode, limitval)       TriggerRegisterUnitStateEvent(trig, unit, UNIT_STATE_LIFE, opcode, limitval)
#  define TriggerRegisterUnitManaEvent(trig, unit, opcode, limitval)       TriggerRegisterUnitStateEvent(trig, unit, UNIT_STATE_MANA, opcode, limitval)
#  define TriggerRegisterDialogEventBJ(trig, dialog)                       TriggerRegisterDialogEvent(trig, dialog)
#  define TriggerRegisterShowSkillEventBJ(trig)                            TriggerRegisterGameEvent(trig, EVENT_GAME_SHOW_SKILL)
#  define TriggerRegisterBuildSubmenuEventBJ(trig)                         TriggerRegisterGameEvent(trig, EVENT_GAME_BUILD_SUBMENU)
#  define TriggerRegisterGameLoadedEventBJ(trig)                           TriggerRegisterGameEvent(trig, EVENT_GAME_LOADED)
#  define TriggerRegisterGameSavedEventBJ(trig)                            TriggerRegisterGameEvent(trig, EVENT_GAME_SAVE)
#
#endif
