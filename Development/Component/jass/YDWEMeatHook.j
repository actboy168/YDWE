#ifndef YDWEMeatHookIncluded
#define YDWEMeatHookIncluded

#include "YDWEBase.j"

library MeatHook requires YDWEBase
//****************************//
//* 技能-钩子 Fetrix_sai修正 *//
//****************************//
    #define MOVE_DIST 40.0
    #define KEEP_DIST 50.0
    globals
        //private constant real MOVE_DIST = 40. //
        //private constant real KEEP_DIST = 50. // 
        //
        private group    Except = CreateGroup()
        private location Point  = Location(0.0, 0.0)
    endglobals
    
    private function GetUnitZ takes unit u returns real
        call MoveLocation(Point, GetUnitX(u), GetUnitY(u))
        return (GetUnitFlyHeight(u) + GetLocationZ(Point))
    endfunction
    
    private function HookAim takes string key, unit pudge, real x, real y, real z returns nothing
        local real damage = YDWEGetRealByString(key, "Damage")
        local group g = CreateGroup()
        local unit  f = null
        call GroupEnumUnitsInRange(g, x, y, 120., null)
        loop
            set f = FirstOfGroup(g)
            exitwhen f == null
            if GetWidgetLife(f) > 0.405 and not(IsUnitType(f, UNIT_TYPE_STRUCTURE)) and not(IsUnitInGroup(f, Except)) and f != pudge then
                if (GetUnitX(f)-x)*(GetUnitX(f)-x)+(GetUnitY(f)-y)*(GetUnitY(f)-y)+(GetUnitZ(f)-z)*(GetUnitZ(f)-z) < 14400 then
                    //* 不幸被钩子勾中了，痛苦地喊出一声“雅蠛蝶~”
                    set bj_lastAbilityTargetUnit = f
                    call YDWESyStemAbilityCastingOverTriggerAction(pudge, 4) 
                    call YDWESaveIntegerByString(key, "Switch", 2)
                    call YDWESaveUnitByString(key, "Target", f)
                    if IsUnitEnemy(f, GetOwningPlayer(pudge))then
                        call DestroyEffect(AddSpecialEffectTarget("Objects\\Spawnmodels\\Human\\HumanBlood\\BloodElfSpellThiefBlood.mdl", f, "origin"))
                        call UnitDamageTarget(pudge, f, damage, true, false, bj_lastSetAttackType, bj_lastSetDamageType, bj_lastSetWeaponType)
                    endif
                    call IssueImmediateOrderById(f, 851972)
                    call GroupAddUnit(Except, f)
                    exitwhen true
                endif
            endif
            call GroupRemoveUnit(g, f)
            set f = null
        endloop
        call DestroyGroup(g)
        set g = null
        set f = null
    endfunction
    
    private function Cycle takes nothing returns nothing
        local string key = I2S(YDWEH2I(GetExpiredTimer()))
        local integer total = YDWEGetIntegerByString(key, "ChainLinkNumber")
        local integer size = YDWEGetIntegerByString(key, "ChainLinkNumberMax")
        local integer utid = YDWEGetIntegerByString(key, "ChainLinkId")
        local integer switch = YDWEGetIntegerByString(key, "Switch")
        local unit pudge = YDWEGetUnitByString(key, "Pudge")
        local unit target = YDWEGetUnitByString(key, "Target")
        local real dist = YDWEGetRealByString(key, "Distance")
        local real l
        local real f
        local real x
        local real y
        local real xvel
        local real yvel
        local unit array part
        local integer i = 0
        
        loop
            exitwhen i == total
            set part[i] = YDWEGetUnitByString(key, "ChainLinks"+I2S(i))
            set i = i + 1
        endloop
        
        //*===== Hook Target =====*
        if target == null then
            call HookAim(key, pudge, GetUnitX(part[0]), GetUnitY(part[0]), GetUnitZ(part[0]))
        else 
            call SetUnitX(target, GetUnitX(part[0]))
            call SetUnitY(target, GetUnitY(part[0]))
            if GetLocalPlayer() == GetOwningPlayer(target) then
                call SelectUnit(target, false)
            endif
        endif
        
        //*===== Stretch =====*
        if switch == 1 then
            //*===== Chain Head =====*
            set x = GetUnitX(part[0]) + YDWEGetRealByString(key, "VelocityX")
            set y = GetUnitY(part[0]) + YDWEGetRealByString(key, "VelocityY")
            call SetUnitX(part[0], x)
            call SetUnitY(part[0], y)
            //call SetUnitFacing(part[i], YDWEGetRealByString(key, "Face"))
            if YDWECoordinateX(x) != x or YDWECoordinateY(y) != y then
                call YDWESaveIntegerByString(key, "Switch", 2)
            endif
            //*===== Chain Links =====*
            set i = 1
            loop
                exitwhen i == total
                set f = Atan2(GetUnitY(part[i-1]) - GetUnitY(part[i]), GetUnitX(part[i-1]) - GetUnitX(part[i]))
                call SetUnitX(part[i], GetUnitX(part[i]) + Cos(f) * MOVE_DIST)
                call SetUnitY(part[i], GetUnitY(part[i]) + Sin(f) * MOVE_DIST)
                call SetUnitFacing(part[i], f * bj_RADTODEG)
                set i = i + 1
            endloop
        elseif switch == 2 then
            set l = (KEEP_DIST + MOVE_DIST) * (KEEP_DIST + MOVE_DIST)
            set i = total - 1
            //*===== Move Back =====*
            loop
                exitwhen i < 0
                if part[i+1] == null then
                    set xvel = GetUnitX(part[i]) - GetUnitX(pudge)
                    set yvel = GetUnitY(part[i]) - GetUnitY(pudge)
                    if xvel * xvel + yvel * yvel <= l then
                        call RemoveUnit(part[i])
                        set part[i] = null
                        set total = total - 1
                        call YDWESaveIntegerByString(key, "ChainLinkNumber", total)
                    endif
                else
                    set xvel = GetUnitX(part[i]) - GetUnitX(part[i+1])
                    set yvel = GetUnitY(part[i]) - GetUnitY(part[i+1])
                endif
                
                if part[i] != null then
                    set f = Atan2(yvel, xvel)
                    call SetUnitX(part[i], GetUnitX(part[i]) - Cos(f) * MOVE_DIST)
                    call SetUnitY(part[i], GetUnitY(part[i]) - Sin(f) * MOVE_DIST)
                    call SetUnitFacing(part[i], f * bj_RADTODEG)
                endif 
                
                set i = i - 1
            endloop

            if total < 1 then
                call YDWESaveIntegerByString(key, "Switch", 0)
                set switch = 0 
            endif
        endif
               
        //*===== Fill Chain =====*
        if switch > 0 /*and size >= total*/ then 
            set x = GetUnitX(part[total-1])
            set y = GetUnitY(part[total-1])
            set xvel = x - GetUnitX(pudge)
            set yvel = y - GetUnitY(pudge)
            set l = SquareRoot(xvel * xvel + yvel * yvel)
            
            if l > dist + KEEP_DIST then
                set f = bj_RADTODEG * Atan2(yvel, xvel)
                set xvel = xvel * (dist / l)
                set yvel = yvel * (dist / l)
                loop
                    exitwhen l < dist
                    if total > size then
                        call YDWESaveIntegerByString(key, "Switch", 2)
                        exitwhen true
                    endif
                    set x = x - xvel
                    set y = y - yvel
                    set part[total] = CreateUnit(GetOwningPlayer(pudge), utid, x, y, f)
                    call YDWESaveUnitByString(key, "ChainLinks"+I2S(total), part[total])
                    set total = total + 1
                    set l = l - dist
                endloop
                call YDWESaveIntegerByString(key, "ChainLinkNumber", total)
            endif
            
            if l > KEEP_DIST + MOVE_DIST * 2 then  
                set l = (KEEP_DIST + MOVE_DIST) / dist
                set xvel = xvel * l
                set yvel = yvel * l
                call SetUnitX(part[total-1], GetUnitX(pudge) + xvel)
                call SetUnitY(part[total-1], GetUnitY(pudge) + yvel)
                call SetUnitFacing(part[total-1], Atan2(yvel, xvel) * bj_RADTODEG)
                
                set i = total - 2
                loop
                    exitwhen i < 0
                    set xvel = GetUnitX(part[i]) - GetUnitX(part[i+1])
                    set yvel = GetUnitY(part[i]) - GetUnitY(part[i+1])
                    set l = dist / SquareRoot(xvel * xvel + yvel * yvel)
                    call SetUnitX(part[i], GetUnitX(part[i+1]) + xvel * l)
                    call SetUnitY(part[i], GetUnitY(part[i+1]) + yvel * l)
                    call SetUnitFacing(part[i], Atan2(yvel, xvel) * bj_RADTODEG)                   
                    set i = i - 1
                endloop
            endif
        endif
        
        //*===== Release =====*
        if switch == 0 then
            call YDWESyStemAbilityCastingOverTriggerAction(pudge, 5)
            call YDWEFlushMissionByString(key)
            call GroupRemoveUnit(Except, target)
            call PauseTimer(GetExpiredTimer())
            call DestroyTimer(GetExpiredTimer())
        endif
        
        set i = 0
        loop
            exitwhen i >= total
            set part[i] = null
            set i = i + 1
        endloop
        
        set target = null
        set pudge = null
        set key = ""
    endfunction
    
    function YDWEMeatHook takes unit pudge, location point, integer headid, integer chainid, real length, real distance, real damage, real timeout returns nothing
        local real  f = Atan2(GetLocationY(point) - GetUnitY(pudge), GetLocationX(point) - GetUnitX(pudge))
        local real  x = Cos(f) * KEEP_DIST
        local real  y = Sin(f) * KEEP_DIST
        local unit  u = CreateUnit(GetOwningPlayer(pudge), headid, GetUnitX(pudge) + x, GetUnitY(pudge) + y, f * bj_RADTODEG)
        local timer t = CreateTimer()
        // *===== Save =====*
        call YDWESaveUnitByString(I2S(YDWEH2I(t)), "Pudge", pudge)
        call YDWESaveUnitByString(I2S(YDWEH2I(t)), "ChainLinks"+I2S(0), u)
        call YDWESaveRealByString(I2S(YDWEH2I(t)), "VelocityX", x * (MOVE_DIST / KEEP_DIST))
        call YDWESaveRealByString(I2S(YDWEH2I(t)), "VelocityY", y * (MOVE_DIST / KEEP_DIST))
        call YDWESaveRealByString(I2S(YDWEH2I(t)), "Face", f * bj_RADTODEG)
        call YDWESaveRealByString(I2S(YDWEH2I(t)), "Distance", distance)
        call YDWESaveIntegerByString(I2S(YDWEH2I(t)), "Switch", 1)
	    call YDWESaveIntegerByString(I2S(YDWEH2I(t)), "ChainLinkId", chainid)
	    call YDWESaveIntegerByString(I2S(YDWEH2I(t)), "ChainLinkNumberMax", R2I(length/distance))
        call YDWESaveIntegerByString(I2S(YDWEH2I(t)), "ChainLinkNumber", 1)
	    call YDWESaveRealByString(I2S(YDWEH2I(t)), "Damage", damage)        
	    call TimerStart(t, timeout, true, function Cycle)
        // *===== end =====*
        set u = null
        set t = null
    endfunction

    #undef MOVE_DIST
    #undef KEEP_DIST
endlibrary
#endif /// YDWEMeatHookIncluded
