#ifndef YDWECreateLineIncluded
#define YDWECreateLineIncluded

#include "YDWEBase.j"

//===========================================================================
//飘动的红丝带
//===========================================================================    
library YDWECreateLine requires YDWEBase
private function Setline takes nothing returns nothing
    local timer t = GetExpiredTimer()          
    local unit tru = YDWEGetUnitByString(I2S(YDWEH2I(t)),"tru")
    local integer count = YDWEGetIntegerByString(I2S(YDWEH2I(t)),"count") 
    local integer steps = YDWEGetIntegerByString(I2S(YDWEH2I(t)),"steps") 
    local integer stepscount = YDWEGetIntegerByString(I2S(YDWEH2I(t)),"stepscount") 
    local integer position = YDWEGetIntegerByString(I2S(YDWEH2I(t)),"position")         
    local real ux = GetUnitX(tru)
    local real uy = GetUnitY(tru)
    local real mjx
    local real mjy
    local real distance = YDWEGetRealByString(I2S(YDWEH2I(t)),"distance")
    local real angleadd = YDWEGetRealByString(I2S(YDWEH2I(t)),"angleadd")
    local unit array tempu
    local real array tempx
    local real array tempy
    local real array angle
    local integer i = 0
    local integer n = 0
    loop
        exitwhen i>count
        set tempu[i] = YDWEGetUnitByString(I2S(YDWEH2I(t)),"unit"+I2S(i))
        set angle[i] = YDWEGetRealByString(I2S(YDWEH2I(t)),"angle"+I2S(i))
        set tempx[i] = YDWEGetRealByString(I2S(YDWEH2I(t)),"unitx"+I2S(i))
        set tempy[i] = YDWEGetRealByString(I2S(YDWEH2I(t)),"unity"+I2S(i))
        set i = i + 1
    endloop
    set i = 0
    if (steps > stepscount) or GetUnitState(tru,UNIT_STATE_LIFE)<=0  then
        loop
            exitwhen i == count
            call RemoveUnit(YDWEGetUnitByString(I2S(YDWEH2I(t)),"unit"+I2S(i)))
            set i = i + 1
        endloop                                                  
        call YDWEFlushMissionByString(I2S(YDWEH2I(t)))
        call PauseTimer(t)
        call DestroyTimer(t)      
        call YDWESyStemAbilityCastingOverTriggerAction(tru,3)        
        //call SetUnitScalePercent( tru, 100, 100, 100 )
        return
    endif
    set i = 0
    loop
        exitwhen i == count
        set n = ModuloInteger(position + i,count)
        set mjx = GetUnitX(tempu[i])
        set mjy = GetUnitY(tempu[i])
        set mjx = mjx + (tempx[n] + i*distance*CosBJ(angle[n]) - mjx) * 0.2
        set mjy = mjy + (tempy[n] + i*distance*SinBJ(angle[n]) - mjy) * 0.2
        call SetUnitX(tempu[i],YDWECoordinateX(mjx))
        call SetUnitY(tempu[i],YDWECoordinateY(mjy))
        set tempu[i]=null
        set i =  i + 1
    endloop
    set steps = steps + 1
    call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"steps",steps)      
    set position = ModuloInteger(position - 1 , count)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"angle"+I2S(position),GetUnitFacing(tru) + 180 + angleadd) 
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"unitx"+I2S(position),ux)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"unity"+I2S(position),uy)        
    call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"position",position)  
    set t=null
    set tru=null
endfunction


//红丝带的先决条件            (udg_cinS_actor[11],'h01J',12,50,60,30,0.03,120,10,100,30)
function YDWECreateLine takes unit tru,integer ut,integer count,real angleadd,real distance,real lasttime,real eachtime,real heightstart,real heightadd,real sizestart,real sizeadd returns nothing
    local timer t = CreateTimer()
    local real ux = GetUnitX(tru)
    local real uy = GetUnitY(tru)
    local real angle = GetUnitFacing(tru) + 180 + angleadd
    local player p = GetOwningPlayer(tru)
    local real percentScale
    local integer i = 0
    local unit tempu
    call YDWESaveUnitByString(I2S(YDWEH2I(t)),"tru",tru)  
    call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"count",count)
    call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"steps",0)
    call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"stepscount",R2I(lasttime/eachtime))
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"distance",distance)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"angleadd",angleadd)    
    call GroupClear(bj_lastCreatedGroup)
    loop
        exitwhen i==count
        set tempu = CreateUnit(p,ut,ux,uy,0)
        call GroupAddUnit(bj_lastCreatedGroup, tempu)
        set bj_lastCreatedUnit=tempu
        call SetUnitFlyHeight(tempu,i*heightadd+heightstart+150, 75 )
		set percentScale = RMaxBJ(i*sizeadd+sizestart,1) * 0.01
        call SetUnitScale(tempu, percentScale, percentScale, percentScale)
        call YDWESaveUnitByString(I2S(YDWEH2I(t)),"unit"+I2S(i),tempu)
        call YDWESaveRealByString(I2S(YDWEH2I(t)),"angle"+I2S(i),angle)
        call YDWESaveRealByString(I2S(YDWEH2I(t)),"unitx"+I2S(i),ux)
        call YDWESaveRealByString(I2S(YDWEH2I(t)),"unity"+I2S(i),uy)
        set tempu=null
        set i = i + 1
    endloop
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"position",0)
    call TimerStart(t,eachtime,true,function Setline)
    set t = null
    set p = null
endfunction
endlibrary

#endif /// YDWECreateLineIncluded
