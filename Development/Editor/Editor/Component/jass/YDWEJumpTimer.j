#ifndef YDWEJumpTimerIncluded
#define YDWEJumpTimerIncluded

#include "YDWEBase.j"

//===========================================================================
//跳跃系统 
//===========================================================================
library YDWEJumpTimer requires YDWEBase

globals
//跳跃系统优先级
	integer MoveMoreLevel_JumpTimer=3
endglobals

function YDWEJumpTimerLoop takes nothing returns nothing
    local timer t=GetExpiredTimer()
    local unit hero=(YDWEGetUnitByString(I2S(YDWEH2I(t)),"Hero"))
    local real angle=YDWEGetRealByString(I2S(YDWEH2I(t)),"Angle")
    local integer steeps=YDWEGetIntegerByString(I2S(YDWEH2I(t)),"steeps")
    local integer steepsMax=YDWEGetIntegerByString(I2S(YDWEH2I(t)),"steepsMax")
    local real heightMax=YDWEGetRealByString(I2S(YDWEH2I(t)),"heightMax")
    local real dist=YDWEGetRealByString(I2S(YDWEH2I(t)),"dist")
    local real dheig=YDWEGetRealByString(I2S(YDWEH2I(t)),"dheig")
    local real OriginHeight=YDWEGetRealByString(I2S(YDWEH2I(t)),"OriginHeight")
    local real x=YDWEGetRealByString(I2S(YDWEH2I(t)),"X")
    local real y=YDWEGetRealByString(I2S(YDWEH2I(t)),"Y")
    local real x1=0
    local real y1=0
    local real height=0  
    local integer lvJumpTimer=MoveMoreLevel_JumpTimer    
    local integer lvUnitMove=YDWEGetIntegerByString(I2S(YDWEH2I(hero)),"lvUnitMove")      
    if steeps<steepsMax and lvJumpTimer>=lvUnitMove then
        set x1=x+steeps*dist*Cos(angle*3.14159/180.0)
        set y1=y+steeps*dist*Sin(angle*3.14159/180.0)
        set x1=YDWECoordinateX(x1)
        set y1=YDWECoordinateY(y1)
        call SetUnitX(hero,x1)
        call SetUnitY(hero,y1)
        set steeps=steeps+1
        call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"steeps",steeps) 
        set height=(-(2*I2R(steeps)*dheig-1)*(2*I2R(steeps)*dheig-1)+1)*heightMax+OriginHeight      
        call SetUnitFlyHeight(hero,height,99999)    
        call SetUnitFacing(hero,angle)             
      else               
        call SetUnitFlyHeight(hero,OriginHeight,99999)
        call SetUnitPathing(hero, true)
        call DestroyTimer(t)
        call YDWEFlushMissionByString(I2S(YDWEH2I(t)))
        call YDWESaveIntegerByString(I2S(YDWEH2I(hero)),"lvUnitMove",0)  
        call YDWESyStemAbilityCastingOverTriggerAction(hero,2) 
    endif
    set t=null
    set hero=null    
endfunction
                            

function YDWEJumpTimer takes unit hero,real angle,real distance,real lasttime,real timeout,real heightMax returns nothing
    local timer t=null
    local real x=GetUnitX(hero)
    local real y=GetUnitY(hero)
    local integer steepsMax=R2I(lasttime/timeout)
    local integer steeps=0
    local real dist=distance/steepsMax     
    local real dheig=1.0/steepsMax
    local real OriginHeight=GetUnitFlyHeight(hero)
    local integer lvUnitMove=YDWEGetIntegerByString(I2S(YDWEH2I(hero)),"lvUnitMove")     
    if MoveMoreLevel_JumpTimer<=lvUnitMove then
        return
    endif    
    set t=CreateTimer()
    call YDWESaveIntegerByString(I2S(YDWEH2I(hero)),"lvUnitMove",MoveMoreLevel_JumpTimer)
    call YDWEFlyEnable(hero)
    call SetUnitPathing(hero,false)          
    call YDWESaveUnitByString(I2S(YDWEH2I(t)),"Hero",hero)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"OriginHeight",OriginHeight)   
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"Angle",angle)    
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"dist",dist)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"heightMax",heightMax)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"dheig",dheig)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"X",x)
    call YDWESaveRealByString(I2S(YDWEH2I(t)),"Y",y)      
    call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"steeps",steeps)  
    call YDWESaveIntegerByString(I2S(YDWEH2I(t)),"steepsMax",steepsMax)   
    call TimerStart(t,timeout,true,function YDWEJumpTimerLoop)
    set t = null
endfunction
endlibrary 

#endif /// YDWEJumpTimerIncluded
