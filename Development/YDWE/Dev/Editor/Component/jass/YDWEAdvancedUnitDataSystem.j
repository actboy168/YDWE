#ifndef YDWEAdvancedUnitDataSystemIncluded
#define YDWEAdvancedUnitDataSystemIncluded

#include "YDWEBase.j"

library YDWEAdvancedUnitDataSystem requires YDWEBase

<?local Misc_DefenseArmor     = tonumber(require('slk').misc.Misc.DefenseArmor)?>

globals
    private constant real    ARMOR_REDUCTION_MULTIPLIER1 = <?=Misc_DefenseArmor?>
    private constant real    ARMOR_REDUCTION_MULTIPLIER2 = <?=1.-Misc_DefenseArmor?>
    private constant real    NATLOG_094                  = <?=string.format('%.6f', math.log(1.-Misc_DefenseArmor))?>
    private constant real    ARMOR_INVULNERABLE          = 917451.519
    private constant real    DAMAGE_TEST                 = 160.
    private constant real    DAMAGE_LIFE                 = 300.
    private integer LIFE_BONUS_SPELL_ID                   = 'A000'
endglobals

globals
#ifndef YDWE_DamageEventTrigger
#define YDWE_DamageEventTrigger
    trigger yd_DamageEventTrigger = null
#endif
endglobals

native GetUnitGoldCost takes integer unitid returns integer
native GetUnitWoodCost takes integer unitid returns integer
native GetUnitBuildTime takes integer unitid returns integer
native GetUnitCountDone takes integer unitid returns integer
constant native GetUnitFoodUsed takes unit whichUnit returns integer
constant native GetUnitFoodMade takes unit whichUnit returns integer

private function Log takes real x returns real
        local real min = -88.0
        local real max = 88.0
        local real mid
        local integer i = 20
        loop
            set mid = (min + max) / 2
            exitwhen(i <= 0)
            set i = i - 1
            if(Pow(bj_E, mid) >= x) then
                set max = mid
            else
                set min = mid
            endif
        endloop
        return mid
    endfunction

private  function Logarithm takes real base, real x returns real
        local real min = -88.0
        local real max = 88.0
        local real mid
        local integer i = 20
        loop
            set mid = (min + max) / 2
            exitwhen(i <= 0)
            set i = i - 1
            if(Pow(base, mid) >= x) then
                set max = mid
            else
                set min = mid
            endif
        endloop
        return mid
    endfunction                   
       
function YDWEGetUnitGoldCost takes integer unitid returns integer
    if IsHeroUnitId(unitid) == true then
        return 425
    endif
    return GetUnitGoldCost(unitid)
endfunction

function YDWEGetUnitWoodCost takes integer unitid returns integer
    if IsHeroUnitId(unitid) == true then
        return 100
    endif
    return GetUnitWoodCost(unitid)
endfunction

function YDWEGetUnitBuildTime takes integer unitid returns integer
    if IsHeroUnitId(unitid) == true then
        return 55
    endif
    return GetUnitBuildTime(unitid)
endfunction

function YDWEGetUnitCountDone takes integer unitid returns integer
    if IsHeroUnitId(unitid) == true then
        return 0
    endif
    return GetUnitCountDone(unitid)
endfunction

function YDWEGetUnitFoodUsed takes unit whichUnit returns integer
    if IsUnitType(whichUnit, UNIT_TYPE_HERO) == true  then
        return 5
    endif
    return GetUnitFoodUsed(whichUnit)
endfunction

function YDWEGetUnitFoodMade takes unit whichUnit returns integer
    if IsUnitType(whichUnit, UNIT_TYPE_HERO) == true then
        return 0
    endif
    return GetUnitFoodMade(whichUnit)
endfunction

function YDWEGetUnitArmor takes unit u,integer aid returns real
    local real    life = GetWidgetLife(u)
    local real    test = life
    local real    redc = 0.
    local boolean enab = false
    local trigger trig = GetTriggeringTrigger()
    set LIFE_BONUS_SPELL_ID=aid
    if u != null and life >= 0.405 then
        if GetUnitState(u, UNIT_STATE_MAX_LIFE) <= DAMAGE_LIFE then
            //Add max life to keep it alive
            call UnitAddAbility(u, LIFE_BONUS_SPELL_ID)
        endif
        if life <= DAMAGE_LIFE then
            //If under the threshold, heal it for the moment
            call SetWidgetLife(u, DAMAGE_LIFE)
            set test = DAMAGE_LIFE 
        endif
        if trig != null and IsTriggerEnabled(trig) then
            //Disable the trigger to prevent it registering with damage detection systems
            call DisableTrigger(trig)
            set enab = true
        endif
		call DisableTrigger(yd_DamageEventTrigger)
        call UnitDamageTarget(u, u, DAMAGE_TEST, true, false, ATTACK_TYPE_CHAOS, DAMAGE_TYPE_NORMAL, null)
		call EnableTrigger(yd_DamageEventTrigger)
        set redc = (DAMAGE_TEST-test+GetWidgetLife(u))/DAMAGE_TEST              
        if enab then
            //Re-enable the trigger
            call EnableTrigger(trig)
        endif
        //Remove the max life ability
        call UnitRemoveAbility(u, LIFE_BONUS_SPELL_ID)
        call SetWidgetLife(u, life)
        set trig = null
        if redc >= 1. then
             //Invulnerable                
            return ARMOR_INVULNERABLE             
        elseif redc < 0. then
            //Negative Armor
            //return -Log(redc+1.)/NATLOG_094           
            return -Log(redc+1.)/NATLOG_094                   
        else
            //Positive Armor
   
            return redc/(ARMOR_REDUCTION_MULTIPLIER1*(1.-redc))
        endif
    endif
    set trig = null
    return 0.
endfunction

private function GetReducedDamage takes real baseDamage, real armor returns real
    if armor >= 0. then
        return baseDamage*(1.-((armor*ARMOR_REDUCTION_MULTIPLIER1)/(1.+ARMOR_REDUCTION_MULTIPLIER1*armor)))
    else
        return baseDamage*(2.-Pow(ARMOR_REDUCTION_MULTIPLIER2,-armor))
    endif
endfunction

private function GetFullDamage takes real damage, real armor returns real
    if armor >= 0. then
        return damage/(1.-((armor*ARMOR_REDUCTION_MULTIPLIER1)/(1.+ARMOR_REDUCTION_MULTIPLIER1*armor)))
    else
        return damage/(2.-Pow(ARMOR_REDUCTION_MULTIPLIER2,-armor))
    endif
endfunction

endlibrary

#endif /// YDWEAdvancedUnitDataSystemIncluded
