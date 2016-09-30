# /*
#  *  BJ函数优化 -- Hero
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_HERO_H
#define INCLUDE_BJ_OPTIMIZATION_HERO_H
#
#  include <YDTrigger/BJOptimization/detail/Hero/GetHeroStatBJ.h> 
#  include <YDTrigger/BJOptimization/detail/Hero/CheckItemStatus.h> 
#  include <YDTrigger/BJOptimization/detail/Hero/CheckItemcodeStatus.h> 
#
#  define GetItemLifeBJ(widget)                                  GetWidgetLife(widget)
#  define SetItemLifeBJ(widget, life)                            SetWidgetLife(widget, life)
#  define AddHeroXPSwapped(xp, hero, show)                       AddHeroXP(hero, xp, show) 
#  define DecUnitAbilityLevelSwapped(abilcode, unit)             DecUnitAbilityLevel(unit, abilcode)
#  define IncUnitAbilityLevelSwapped(abilcode, unit)             IncUnitAbilityLevel(unit, abilcode)
#  define SetUnitAbilityLevelSwapped(abilcode, unit, level)      SetUnitAbilityLevel(unit, abilcode, level)
#  define GetUnitAbilityLevelSwapped(abilcode, unit)             GetUnitAbilityLevel(unit, abilcode)
#  define UnitRemoveBuffBJ(abilcode, unit)                       UnitRemoveAbility(unit, abilcode)
#  define UnitAddItemSwapped(item, hero)                         UnitAddItem(hero, item)
#  define UnitRemoveItemSwapped(item, hero)                      UnitRemoveItem(hero, item) YDNL set bj_lastRemovedItem = item
#  define GetLearnedSkillBJ()                                    GetLearnedSkill()
#  define SuspendHeroXPBJ(hero, flag)                            SuspendHeroXP(hero, not (flag))
#  define SetPlayerHandicapXPBJ(player, handicapPercent)         SetPlayerHandicapXP(player, (handicapPercent) * 0.01)
#  define SetPlayerHandicapBJ(player, handicapPercent)           SetPlayerHandicap(player, (handicapPercent) * 0.01)
#  define GetLearnedSkillBJ()                                    GetLearnedSkill()
#  define UnitDropItemPointBJ(unit, item, x, y)                  UnitDropItemPoint(unit, item, x, y)
#  define UnitDropItemSlotBJ(unit, item, slot)                   UnitDropItemSlot(unit, item, (slot)-1)
#  define UnitDropItemTargetBJ(unit, item, target)               UnitDropItemTarget(unit, item, target)
#  define UnitUseItemDestructable(unit, item, target)            UnitUseItemTarget(unit, item, target)
#  define UnitItemInSlotBJ(unit, slot)                           UnitItemInSlot(unit, (slot)-1)
#  define UnitInventorySizeBJ(unit)                              UnitInventorySize(unit)
#  define SetItemInvulnerableBJ(item, flag)                      SetItemInvulnerable(item, flag)
#  define SetItemDropOnDeathBJ(item, flag)                       SetItemDropOnDeath(item, flag)
#  define SetItemDroppableBJ(item, flag)                         SetItemDroppable(item, flag)
#  define SetItemPlayerBJ(item, player, changeColor)             SetItemPlayer(item, player, changeColor)
#  define SetItemVisibleBJ(show, item)                           SetItemVisible(item, show)
#  define ChooseRandomItemBJ(level)                              ChooseRandomItem(level)
#  define ChooseRandomItemExBJ(level, type)                      ChooseRandomItemEx(type, level)
#  define ChooseRandomNPBuildingBJ()                             ChooseRandomNPBuilding()
#  define ChooseRandomCreepBJ(level)                             ChooseRandomCreep(level)
#  define EnumItemsInRectBJ(r, actionFunc)                       EnumItemsInRect(r, null, actionFunc)
#  define RandomItemInRectSimpleBJ(r)                            RandomItemInRectBJ(r, null)

#
# /*
#  *  有返回值的函数, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define UnitHasBuffBJ(unit, buffcode)                          (GetUnitAbilityLevel(unit, buffcode) > 0)
#  define GetLastCreatedItem()                                   bj_lastCreatedItem
#  define GetLastRemovedItem()                                   bj_lastRemovedItem
#  define GetPlayerHandicapXPBJ(player)                          (GetPlayerHandicapXP(player) * 100)
#  define GetPlayerHandicapBJ(player)                            (GetPlayerHandicap(player) * 100)
#  define UnitHasItemOfTypeBJ(unit, itemId)                      (GetInventoryIndexOfItemTypeBJ(unit, itemId) > 0)
#  define IsItemHiddenBJ(item)                                   (not IsItemVisible(item))
#
#endif
