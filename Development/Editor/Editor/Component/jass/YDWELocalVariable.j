#ifndef YDWELocalVariableIncluded
#define YDWELocalVariableIncluded

#include "YDWEBase.j"

library YDWELocalVariable requires YDWEBase

function YDWELocalVariableInitiliation takes nothing returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"GlobalsTriggerRunSteps")
    set TriggerRunSteps=TriggerRunSteps+1
    call YDWESaveIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"GlobalsTriggerRunSteps",TriggerRunSteps)
    call YDWESaveIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps",TriggerRunSteps)
endfunction

function YDWEWaitForLocalVariable takes real time returns nothing
    local integer TriggerRunSteps=YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
// call BJDebugMsg("等待之前TriggerRunSteps= "+I2S(TriggerRunSteps))
    call TriggerSleepAction(time)
// call BJDebugMsg("等待之后TriggerRunSteps= "+I2S(TriggerRunSteps))
    call YDWESaveIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps",TriggerRunSteps)
endfunction

function YDWELocalVariableEnd takes nothing returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWEFlushMissionByString(dir)
endfunction

//integer
function YDWESetLocalVariableInteger takes string s,integer i returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s,i)
endfunction

function YDWEGetLocalVariableInteger takes string s returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s)
endfunction

function YDWESetLocalVariableIntegerArray takes string s,integer index,integer i returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s+I2S(index),i)
endfunction

function YDWEGetLocalVariableIntegerArray takes string s,integer index returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s+I2S(index))
endfunction

//real
function YDWESetLocalVariableReal takes string s,real R returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveRealByString(dir,s,R)
endfunction

function YDWEGetLocalVariableReal takes string s returns real
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetRealByString(dir,s)
endfunction

function YDWESetLocalVariableRealArray takes string s,integer index,real R returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveRealByString(dir,s+I2S(index),R)
endfunction

function YDWEGetLocalVariableRealArray  takes string s,integer index returns real
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetRealByString(dir,s+I2S(index))
endfunction
//string
function YDWESetLocalVariableString takes string s,string str returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveStringByString(dir,s,str)
endfunction

function YDWEGetLocalVariableString takes string s returns string
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetStringByString(dir,s)
endfunction

function YDWESetLocalVariableStringArray takes string s,integer index,string str returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveStringByString(dir,s+I2S(index),str)
endfunction

function YDWEGetLocalVariableStringArray takes string s,integer index returns string
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetStringByString(dir,s+I2S(index))
endfunction
//boolean
function YDWESetLocalVariableBoolean takes string s,boolean bl returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveBooleanByString(dir,s,bl)
endfunction

function YDWEGetLocalVariableBoolean takes string s returns boolean
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetBooleanByString(dir,s)
endfunction

function YDWESetLocalVariableBooleanArray takes string s,integer index,boolean bl returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveBooleanByString(dir,s+I2S(index),bl)
endfunction

function YDWEGetLocalVariableBooleanArray takes string s,integer index returns boolean
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetBooleanByString(dir,s+I2S(index))
endfunction
//unit
function YDWESetLocalVariableUnit takes string s,unit u returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveUnitByString(dir,s,u)
endfunction

function YDWEGetLocalVariableUnit takes string s returns unit
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetUnitByString(dir,s)
endfunction

function YDWESetLocalVariableUnitArray takes string s,integer index,unit u returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveUnitByString(dir,s+I2S(index),u)
endfunction

function YDWEGetLocalVariableUnitArray takes string s,integer index returns unit
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetUnitByString(dir,s+I2S(index))
endfunction

//unit code
function YDWESetLocalVariableUnitID takes string s,integer uid returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s,uid)
endfunction

function YDWEGetLocalVariableUnitID takes string s returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s)
endfunction

function YDWESetLocalVariableUnitIDArray takes string s,integer index,integer uid returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s+I2S(index),uid)
endfunction

function YDWEGetLocalVariableUnitIDArray takes string s,integer index returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s+I2S(index))
endfunction
//ability code
function YDWESetLocalVariableAbilityID takes string s,integer abid returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s,abid)
endfunction

function YDWEGetLocalVariableAbilityID takes string s returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s)
endfunction

function YDWESetLocalVariableAbilityIDArray takes string s,integer index,integer abid returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s+I2S(index),abid)
endfunction

function YDWEGetLocalVariableAbilityIDArray takes string s,integer index returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s+I2S(index))
endfunction
//item
function YDWESetLocalVariableItem takes string s,item it returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveItemByString(dir,s,it)
endfunction

function YDWEGetLocalVariableItem takes string s returns item
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetItemByString(dir,s)
endfunction

function YDWESetLocalVariableItemArray takes string s,integer index,item it returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveItemByString(dir,s+I2S(index),it)
endfunction

function YDWEGetLocalVariableItemArray takes string s,integer index returns item
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetItemByString(dir,s+I2S(index))
endfunction
//item code
function YDWESetLocalVariableItemID takes string s,integer itid returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s,itid)
endfunction

function YDWEGetLocalVariableItemID takes string s returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s)
endfunction

function YDWESetLocalVariableItemIDArray takes string s,integer index,integer itid returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveIntegerByString(dir,s+I2S(index),itid)
endfunction

function YDWEGetLocalVariableItemIDArray takes string s,integer index returns integer
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetIntegerByString(dir,s+I2S(index))
endfunction
//location
function YDWESetLocalVariableLocation takes string s,location point returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveLocationByString(dir,s,point)
endfunction

function YDWEGetLocalVariableLocation takes string s returns location
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetLocationByString(dir,s)
endfunction

function YDWESetLocalVariableLocationArray takes string s,integer index,location point returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveLocationByString(dir,s+I2S(index),point)
endfunction

function YDWEGetLocalVariableLocationArray takes string s,integer index returns location
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetLocationByString(dir,s+I2S(index))
endfunction
//group
function YDWESetLocalVariableGroup takes string s,group g returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveGroupByString(dir,s,g)
endfunction

function YDWEGetLocalVariableGroup takes string s returns group
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetGroupByString(dir,s)
endfunction

function YDWESetLocalVariableGroupArray takes string s,integer index,group g returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveGroupByString(dir,s+I2S(index),g)
endfunction

function YDWEGetLocalVariableGroupArray takes string s,integer index returns group
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetGroupByString(dir,s+I2S(index))
endfunction
//effect
function YDWESetLocalVariableEffect takes string s,effect e returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveEffectByString(dir,s,e)
endfunction

function YDWEGetLocalVariableEffect takes string s returns effect
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetEffectByString(dir,s)
endfunction

function YDWESetLocalVariableEffectArray takes string s,integer index,effect e returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveEffectByString(dir,s+I2S(index),e)
endfunction

function YDWEGetLocalVariableEffectArray takes string s,integer index returns effect
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetEffectByString(dir,s+I2S(index))
endfunction

//rect
function YDWESetLocalVariableRect takes string s,rect rt returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveRectByString(dir,s,rt)
endfunction

function YDWEGetLocalVariableRect takes string s returns rect
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetRectByString(dir,s)
endfunction

function YDWESetLocalVariableRectArray takes string s,integer index,rect rt returns nothing
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    call YDWESaveRectByString(dir,s+I2S(index),rt)
endfunction

function YDWEGetLocalVariableRectArray takes string s,integer index returns rect
    local integer TriggerRunSteps = YDWEGetIntegerByString(I2S(YDWEH2I(GetTriggeringTrigger())),"TriggerRunSteps")
    local string dir = I2S(YDWEH2I(GetTriggeringTrigger()))+I2S(TriggerRunSteps)
    return YDWEGetRectByString(dir,s+I2S(index))
endfunction


endlibrary 

#endif /// YDWELocalVariableIncluded
