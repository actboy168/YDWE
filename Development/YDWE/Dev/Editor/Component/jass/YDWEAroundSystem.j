#ifndef YDWEAroundSystemIncluded
#define YDWEAroundSystemIncluded

#include "YDWEBase.j"

//===========================================================================  
//万能环绕模板 
//===========================================================================
library YDWEAroundSystem requires YDWEBase
//library TP1 requires YDWEBase
    globals
        private constant timer AROUND_TIM   = CreateTimer()
        private constant real  AROUND_INTER = 0.01
    endglobals

    private struct Data
        static thistype array Structs
        static integer Total = 0
        unit caster = null
        unit obj    = null        
        real dur   = 0.
        real inter = 0.
        real each  = 0.        
        real rate = 0.
        real dis  = 0.
        real rise = 0.        
        real angle  = 0.
        real radius = 0.
        real height = 0
    endstruct

    private function spin takes nothing returns nothing
        local Data d = 0
        local real x = 0.
        local real y = 0.
        local integer inst = 0
        
        loop
            exitwhen (inst == Data.Total)
            set d = Data.Structs[inst]
            if ( d.dur > 0 ) and (GetUnitState(d.caster, UNIT_STATE_LIFE)>0) and (GetUnitState(d.obj, UNIT_STATE_LIFE)>0) then
                set d.each = d.each + AROUND_INTER
                if ( d.each >= d.inter ) then
                    set d.angle  = d.angle  + d.rate
                    set d.radius = d.radius + d.dis
                    set d.height = d.height + d.rise
                    set x = GetUnitX(d.caster) + d.radius*Cos(d.angle)
                    set y = GetUnitY(d.caster) + d.radius*Sin(d.angle)
                    set x = YDWECoordinateX(x)
                    set y = YDWECoordinateY(y)
                    call SetUnitX(d.obj, x)
                    call SetUnitY(d.obj, y)
                    call SetUnitFlyHeight(d.obj, d.height, 0.)
                    set d.each = 0.
                endif
                set d.dur = d.dur - AROUND_INTER
            else                                                      
                set bj_lastAbilityTargetUnit=d.caster
                call YDWESyStemAbilityCastingOverTriggerAction(d.obj,10)   
                set d.caster = null
                set d.obj = null
                call d.destroy()
                set Data.Total = Data.Total - 1
                set Data.Structs[inst] = Data.Structs[Data.Total]
                set inst = inst - 1
            endif
            set inst = inst + 1
        endloop
        if ( Data.Total == 0 ) then
            call PauseTimer(AROUND_TIM)
        endif
    endfunction

    function YDWEAroundSystem takes unit satellite, unit star, real angleRate, real displacement, real riseRate,real timeout, real interval returns nothing
        local Data d = Data.create()
        local real x1 = GetUnitX(star)
        local real y1 = GetUnitY(star)
        local real x2 = GetUnitX(satellite)
        local real y2 = GetUnitY(satellite)
        set d.caster = star
        set d.obj    = satellite
        set d.dur    = timeout
        set d.inter  = interval
        set d.rate   = angleRate*(3.14159/180.)
        set d.dis    = displacement
        set d.rise   = riseRate
        set d.angle  = Atan2(y2-y1,x2-x1)
        set d.radius = SquareRoot((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))
        set d.height = GetUnitFlyHeight(d.obj)        
        set Data.Structs[Data.Total] = integer(d)
        set Data.Total = Data.Total + 1           
        if ( Data.Total - 1 == 0 ) then
            call TimerStart(AROUND_TIM, AROUND_INTER, true, function spin)
        endif
    endfunction
    
endlibrary 


#endif /// YDWEAroundSystemIncluded
