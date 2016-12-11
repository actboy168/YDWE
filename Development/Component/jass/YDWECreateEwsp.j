#ifndef YDWECreateEwspIncluded
#define YDWECreateEwspIncluded

#include "YDWEBase.j"

library YDWECreateEwsp requires YDWEBase
//===========================================================================
//环绕技能模板 
//===========================================================================
private function Loop takes nothing returns nothing
    local timer t = GetExpiredTimer()
	local string h = I2S(YDWEH2I(t))
    local unit tempUnit 
    local real angle 
    local integer i
    local unit orderUnit=YDWEGetUnitByString(h,"orderUnit")     
    local real UnitLocX = GetUnitX(orderUnit)
    local real UnitLocY = GetUnitY(orderUnit) 
    local real radius = YDWEGetRealByString(h,"radius")    
    local real speed = YDWEGetRealByString(h,"speed") 
    local integer number = YDWEGetIntegerByString(h,"number") 
    local integer steps = YDWEGetIntegerByString(h,"steps") 
    if steps>0 and GetUnitState(orderUnit, UNIT_STATE_LIFE)>0 then     
        set steps=steps-1
        call  YDWESaveIntegerByString(h,"steps",steps)  
        set i = 0                
        loop
            set i = i + 1
            exitwhen i > number
            set tempUnit=YDWEGetUnitByString(h,"units"+I2S(i)) 
            set angle=YDWEGetRealByString(h,"angles"+I2S(i)) 
            set angle=angle+speed
            call YDWESaveRealByString(h,"angles"+I2S(i),angle)
            call SetUnitX(tempUnit, YDWECoordinateX(UnitLocX + radius*Cos(angle)))
            call SetUnitY(tempUnit, YDWECoordinateY(UnitLocY + radius*Sin(angle)))
        endloop
    else 
        set i = 0
        loop
            set i = i + 1 
            exitwhen i > number            
            call RemoveUnit(YDWEGetUnitByString(h,"units"+I2S(i)))                     
        endloop 
        call YDWEFlushMissionByString(h)
        call DestroyTimer(t)    
        call YDWESyStemAbilityCastingOverTriggerAction(orderUnit,1)    
    endif
    set tempUnit=null  
    set orderUnit=null
    set t=null
endfunction

function YDWECreateEwsp takes unit Hero,integer ewsp,integer number,real radius,real lasttime,real interval,real speed returns nothing
    local timer t = CreateTimer()
	local string h = I2S(YDWEH2I(t))
	local real UnitLocX = GetUnitX(Hero)
    local real UnitLocY = GetUnitY(Hero)  
    local unit tempUnit  
    local player Masterplayer = GetOwningPlayer(Hero)
    local real angle 
    local integer i = 0
    local integer steps = R2I(lasttime/interval)
    call YDWESaveUnitByString(h,"orderUnit",Hero) 
    call YDWESaveIntegerByString(h,"steps",steps)      
    call YDWESaveIntegerByString(h,"number",number) 
    call YDWESaveRealByString(h,"radius",radius)
    call YDWESaveRealByString(h,"speed", speed*bj_DEGTORAD)
    call GroupClear(bj_lastCreatedGroup)
    loop
        set i = i + 1
        exitwhen i > number
        set angle = 2*i*bj_PI/number
        call YDWESaveRealByString(h,"angles"+I2S(i),angle)
        set tempUnit = CreateUnit(Masterplayer, ewsp, YDWECoordinateX(UnitLocX + radius*Cos(angle)), YDWECoordinateY(UnitLocY + radius*Sin(angle)), angle*bj_RADTODEG)
        call YDWESaveUnitByString(h,"units"+I2S(i),tempUnit)
        call UnitIgnoreAlarm(tempUnit, true)
        call GroupAddUnit(bj_lastCreatedGroup, tempUnit)
        set bj_lastCreatedUnit = tempUnit
    endloop 
    call TimerStart(t,interval,true,function Loop)
    set t=null
    set tempUnit=null
endfunction

endlibrary

#endif /// YDWECreateEwspIncluded
