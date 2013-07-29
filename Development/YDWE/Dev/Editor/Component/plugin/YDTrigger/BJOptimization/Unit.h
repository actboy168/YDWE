# /*
#  *  BJ函数优化 -- Unit
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_UNIT_H
#define INCLUDE_BJ_OPTIMIZATION_UNIT_H
#                                                        
#  include <YDTrigger/BJOptimization/detail/UnitRemoveBuffs.h> 
#  include <YDTrigger/BJOptimization/detail/UnitRemoveBuffsEx.h>
#  include <YDTrigger/BJOptimization/detail/UnitCountBuffsEx.h>
#
#  define String2UnitIdBJ(unitIdString)                          UnitId(unitIdString)
#  define GetIssuedOrderIdBJ()                                   GetIssuedOrderId()
#  define GetKillingUnitBJ()                                     GetKillingUnit()
#  define UnitSuspendDecayBJ(suspend, unit)                      UnitSuspendDecay(unit, suspend)
#  define GetUnitStateSwap(state, unit)                          GetUnitState(unit, state)
#  define GetUnitLifePercent(unit)                               GetUnitStatePercent(whichUnit, UNIT_STATE_LIFE, UNIT_STATE_MAX_LIFE)
#  define GetUnitManaPercent(unit)                               GetUnitStatePercent(whichUnit, UNIT_STATE_MANA, UNIT_STATE_MAX_MANA)
#  define SelectUnitSingle(unit)                                 ClearSelection() YDNL call SelectUnit(unit, true)
#  define SelectGroupBJ(group)                                   ClearSelection() YDNL call ForGroup(group, function SelectGroupBJEnum)
#  define SelectUnitAdd(unit)                                    SelectUnit(unit, true)
#  define SelectUnitRemove(unit)                                 SelectUnit(unit, false)
#  define SetUnitLifeBJ(unit, value)                             SetUnitState(unit, UNIT_STATE_LIFE, RMaxBJ(0, value))
#  define SetUnitManaBJ(unit, value)                             SetUnitState(unit, UNIT_STATE_LIFE, RMaxBJ(0, value))
#  define IsUnitHiddenBJ(unit)                                   IsUnitHidden(unit)
#  define ShowUnitHide(unit)                                     ShowUnit(unit, false)
#  define IssueTrainOrderByIdBJ(unit, id)                        IssueImmediateOrderById(unit, id)  
#  define GroupTrainOrderByIdBJ(group, id)                       GroupImmediateOrderById(group, id)  
#  define IssueUpgradeOrderByIdBJ(unit, id)                      IssueUpgradeOrderById(unit, id)      
#  define GetAttackedUnitBJ()                                    GetTriggerUnit()
#  define SetUnitFlyHeightBJ(unit, height, rate)                 SetUnitFlyHeight(unit, height, rate)
#  define SetUnitTurnSpeedBJ(unit, speed)                        SetUnitTurnSpeed(unit, speed)
#  define GetUnitDefaultPropWindowBJ(unit)                       GetUnitDefaultPropWindow(unit)
#  define SetUnitBlendTimeBJ(unit, time)                         SetUnitBlendTime(unit, time)
#  define SetUnitAcquireRangeBJ(unit, range)                     SetUnitAcquireRange(unit, range)
#  define UnitSetCanSleepBJ(unit, canSleep)                      UnitAddSleep(unit, canSleep)
#  define UnitCanSleepBJ(unit)                                   UnitCanSleep(unit)
#  define UnitWakeUpBJ(unit)                                     UnitWakeUp(unit)
#  define UnitIsSleepingBJ(unit)                                 UnitIsSleeping(unit)
#  define UnitIsSleepingBJ(unit)                                 UnitIsSleeping(unit)
#  define UnitGenerateAlarms(unit, generate)                     UnitIgnoreAlarm(unit, not generate)
#  define PauseUnitBJ(pause, unit)                               PauseUnit(unit, pause)
#  define IsUnitPausedBJ(unit)                                   IsUnitPaused(unit)
#  define UnitPauseTimedLifeBJ(flag, unit)                       UnitPauseTimedLife(unit, flag)
#  define UnitApplyTimedLifeBJ(duration, buffId, unit)           UnitApplyTimedLife(unit, buffId, duration)
#  define UnitShareVisionBJ(share, unit, player)                 UnitShareVision(unit, player, share)
#  define UnitRemoveAbilityBJ(abilityId, unit)                   UnitRemoveAbility(unit, abilityId)
#  define UnitAddAbilityBJ(abilityId, unit)                      UnitAddAbility(unit, abilityId)
#  define UnitRemoveTypeBJ(type, unit)                           UnitRemoveType(unit, type)
#  define UnitAddTypeBJ(type, unit)                              UnitAddType(unit, type)
#  define UnitMakeAbilityPermanentBJ(permanent, abilityId, unit) UnitMakeAbilityPermanent(unit, permanent, abilityId)
#  define SetUnitExplodedBJ(unit, exploded)                      SetUnitExploded(unit, exploded)
#  define GetTransportUnitBJ()                                   GetTransportUnit()
#  define GetLoadedUnitBJ()                                      GetLoadedUnit()
#  define IsUnitInTransportBJ(unit, transport)                   IsUnitInTransport(unit, transport)
#  define IsUnitLoadedBJ(unit)                                   IsUnitLoaded(unit)
#  define IsUnitIllusionBJ(unit)                                 IsUnitIllusion(unit)
#  define AddItemToStockBJ(itemId, unit, currentStock, stockMax) AddItemToStock(unit, itemId, currentStock, stockMax)
#  define AddUnitToStockBJ(unitId, unit, currentStock, stockMax) AddUnitToStock(unit, unitId, currentStock, stockMax)
#  define RemoveItemFromStockBJ(itemId, unit)                    RemoveItemFromStock(unit, itemId)
#  define RemoveUnitFromStockBJ(unitId, unit)                    RemoveItemFromStock(unit, unitId)
#  define SetUnitUseFoodBJ(enable, unit)                         SetUnitUseFood(unit, enable)
#  define UnitDamageTargetBJ(unit, target, amount, attacktype, damagetype) \
            UnitDamageTarget(unit, target, amount, true, false, attacktype, damagetype, WEAPON_TYPE_WHOKNOWS)
#
# /*
#  *  有返回值的函数替换为全局变量, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define UnitId2OrderIdBJ(unitId)                               (unitId)
#  define GetLastCreatedUnit()                                   bj_lastCreatedUnit
#  define GetLastReplacedUnitBJ()                                bj_lastReplacedUnit
#  define DoesUnitGenerateAlarms(unit)                           (not UnitIgnoreAlarmToggled(unit))
#  define IsUnitDeadBJ(unit)                                     (GetUnitState(unit, UNIT_STATE_LIFE) <= 0)
#  define IsUnitAliveBJ(unit)                                    (GetUnitState(unit, UNIT_STATE_LIFE) >  0)
#  define GetUnitPropWindowBJ(unit)                              (GetUnitPropWindow(unit)*bj_RADTODEG)
#
#endif
