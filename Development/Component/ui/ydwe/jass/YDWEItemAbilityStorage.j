#ifndef YDWEItemAbilitySystemIncluded
#define YDWEItemAbilitySystemIncluded

#include "YDWEBase.j"

library YDWEItemAbilitySystem initializer YDWEItemAbilitySystemInit requires YDWEBase

//===========================================================================
//物品属性系
//===========================================================================
function YDWEItemAbilityStorage takes integer itid,integer ab returns nothing
    local integer index = YDWEGetIntegerByString(I2S(itid),"AbilityStorageIndex")
    set index = index + 1
    call YDWESaveIntegerByString(I2S(itid),"AbilityStorage"+I2S(index),ab)
    call YDWESaveIntegerByString(I2S(itid),"AbilityStorageIndex",index)
endfunction

//===========================================================================
//拾取物品触发
//===========================================================================
function YDWEItemAbilitySystemByAbility_CON takes nothing returns boolean
    local unit hero = GetTriggerUnit()
    local item it=GetManipulatedItem()
    local trigger trg=YDWEGetTriggerByString(I2S(YDWEH2I(hero)), "MulItemSlotByAbility")
    local integer itid=GetItemTypeId(it)
    local integer index = YDWEGetIntegerByString(I2S(itid),"AbilityStorageIndex")
    local integer array skills
    local integer i=0
    local integer N=YDWEGetIntegerByString(I2S(YDWEH2I(trg)), "ItemSlotNum")
    local boolean OnOff=YDWEGetBooleanByString(I2S(YDWEH2I(hero)), "YDWEItemAbilitySystemTriggerClose")
    if index==0 or OnOff==true then
        set it=null
        set hero=null
        set trg=null
        return false
    endif
    loop
        set i=i+1
        exitwhen i>index
        set skills[i] = YDWEGetIntegerByString(I2S(itid),"AbilityStorage"+I2S(i))
        if GetTriggerEventId()==EVENT_PLAYER_UNIT_DROP_ITEM then    
            call UnitRemoveAbility(hero,skills[i])
          elseif GetTriggerEventId()==EVENT_PLAYER_UNIT_PICKUP_ITEM then
            call UnitAddAbility(hero,skills[i])
            call UnitMakeAbilityPermanent(hero,true,skills[i])
        endif
    endloop
    set it=null
    set hero=null
    set trg=null
    return false
endfunction

 function YDWEItemAbilitySystemInit takes nothing returns nothing
    local trigger trg = CreateTrigger()
    call TriggerRegisterAnyUnitEventBJ(trg,EVENT_PLAYER_UNIT_PICKUP_ITEM)
    call TriggerRegisterAnyUnitEventBJ(trg,EVENT_PLAYER_UNIT_DROP_ITEM)
    call TriggerAddCondition(trg, Condition(function YDWEItemAbilitySystemByAbility_CON))
    set trg=null
endfunction

endlibrary

#endif
