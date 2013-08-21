#ifndef YDWESetGuardIncluded
#define YDWESetGuardIncluded

#include "YDWEBase.j"

//===========================================================================
//Ó¶±øÏµÍ³ 
//===========================================================================
library YDWESetGuard requires YDWEBase
private function IsUnitIdle takes unit u returns boolean
    return OrderId2String(GetUnitCurrentOrder(u)) == null
endfunction

function YDWERemoveGuard takes unit pet returns nothing
    local integer tm = YDWEGetIntegerByString( I2S(YDWEH2I(pet)), "Timer")
    call YDWEFlushMissionByString(I2S(YDWEH2I(pet)))
    call YDWEFlushMissionByString(I2S(tm))
    call DestroyTimer(YDWEGetTimerByString(I2S(YDWEH2I(pet)), "Timer"))
endfunction

function SetGuardTimer takes nothing returns nothing
  local timer tm = GetExpiredTimer()
  local unit pet = (YDWEGetUnitByString( I2S(YDWEH2I(tm)), "Pet"))
  local unit captain = (YDWEGetUnitByString( I2S(YDWEH2I(tm)), "Captain"))
  local real x = GetUnitX(captain) - GetUnitX(pet)
  local real y = GetUnitY(captain) - GetUnitY(pet)
  local real d = x*x + y*y
  local real v
  local real a
  local effect e=null
  local real life = YDWEGetRealByString( I2S(YDWEH2I(tm)), "Life")
  local integer p = YDWEGetIntegerByString(I2S(YDWEH2I(tm)), "Percent")
  set v = YDWEGetRealByString(I2S(YDWEH2I(tm)), "GuardRanger")      
  if GetUnitState(pet, UNIT_STATE_LIFE)>0 and GetUnitState(captain, UNIT_STATE_LIFE)> 0 then   
      if d<v*v then
         if IsUnitIdle(pet) and GetRandomInt(0,100)<p then
           set x = GetUnitX(captain)
           set y = GetUnitY(captain)
           set d = GetRandomReal(0,v)
           set a = GetRandomReal(0,360)
           call IssuePointOrder(pet, "patrol", x+d*CosBJ(a), y+d*SinBJ(a))
         endif
      else
        set v = YDWEGetRealByString( I2S(YDWEH2I(tm)), "ReturnRanger")
        if d<v*v then
          if IsUnitIdle(pet) then
            call IssuePointOrder(pet, "patrol", GetUnitX(captain), GetUnitY(captain))
          endif
        else
          set v = YDWEGetRealByString(I2S(YDWEH2I(tm)), "OutRanger")
            if d!=0 and d>v*v then
              call SetUnitPosition(pet,GetUnitX(captain),GetUnitY(captain))
              set e =AddSpecialEffectTarget("Abilities\\Spells\\Human\\MassTeleport\\MassTeleportTarget.mdl" ,captain,"chest")
              call DestroyEffect(e)
            else
              call IssuePointOrder(pet, "move", GetUnitX(captain), GetUnitY(captain))
            endif
          endif
       endif
     else
       call IssuePointOrder(pet, "attack", GetUnitX(captain), GetUnitY(captain))
       call YDWERemoveGuard(pet)   
  endif
  set tm = null
  set pet = null
  set captain = null
  set e=null
endfunction


function YDWESetGuard takes unit pet, unit captain, real timeout, real guardRanger, real returnRanger, real outRanger,integer percent returns nothing
    local timer tm = CreateTimer()  
    call YDWESaveTimerByString(I2S(YDWEH2I(pet)), "Timer", tm)
    call YDWESaveUnitByString(I2S(YDWEH2I(tm)), "pet", pet)
    call YDWESaveUnitByString(I2S(YDWEH2I(tm)), "Captain", captain)
    call YDWESaveIntegerByString(I2S(YDWEH2I(tm)), "Percent", percent)   
    call YDWESaveRealByString(I2S(YDWEH2I(tm)), "GuardRanger", guardRanger)    
    call YDWESaveRealByString(I2S(YDWEH2I(tm)), "ReturnRanger", returnRanger)   
    call YDWESaveRealByString(I2S(YDWEH2I(tm)), "OutRanger", outRanger)
    call TimerStart(tm, timeout, true, function SetGuardTimer)
    set tm = null
endfunction
endlibrary 

#endif /// YDWESetGuardIncluded
