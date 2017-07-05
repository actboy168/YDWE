#ifndef YDWEGeneralBounsSystemIncluded
#define YDWEGeneralBounsSystemIncluded

//===========================================================================
//修改生命
//===========================================================================

#include "YDWEBase.j"
library YDWEGeneralBounsSystem initializer Initialize

globals
    private integer array ABILITY_COUNT
    private integer array ABILITY_NUM
    private integer array BonusAbilitys
    private integer array PowersOf2
    private integer PRELOAD_DUMMY_UNIT = 'hpea'
    private integer array YDWEBONUS_MyChar
    private boolean PRELOAD_ABILITYS = true
    private constant integer BONUS_TYPES = 4
    private integer array MaxBonus
    private integer array MinBonus
    private unit array Units
    private integer UnitCount = 0
endglobals

<?
local idx = {
'0','1','2','3','4','5','6','7','8','9',
'a','b','c','d','e','f','g',
'h','i','j','k','l','m','n',
}
local slk = require 'slk'
for i = 1, 24 do
	local o = slk.ability.AItx:new('YDb'..idx[i])
	local v = 2^(i-1)
	if i == 24 then
		v = -v
	end
	o.DataA1 = v
end
for i = 1, 13 do
	local o = slk.ability.AId0:new('YDc'..idx[i])
	local v = 2^(i-1)
	if i == 13 then
		v = -v
	end
	o.DataA1 = v
end
for i = 1, 16 do
	local o = slk.ability.AIlz:new('YDl'..idx[i])
	local v = 2^(i-1)
	o.levels = 3
	o.DataA1 = 0
	o.DataA2 = v
	o.DataA3 = -v
end
for i = 1, 16 do
	local o = slk.ability.AImz:new('YDm'..idx[i])
	local v = 2^(i-1)
	o.levels = 3
	o.DataA1 = 0
	o.DataA2 = v
	o.DataA3 = -v
end
?>
//以下函数仅仅是让技能ID出现在代码里，不然SLK优化器会删除这些技能
private function DisplayAllAbilityId takes nothing returns nothing
    local integer aid=0
    set aid='YDl0'
    set aid='YDl1'
    set aid='YDl2'
    set aid='YDl3'
    set aid='YDl4'
    set aid='YDl5'
    set aid='YDl6'
    set aid='YDl7'
    set aid='YDl8'
    set aid='YDl9'
    set aid='YDla'
    set aid='YDlb'
    set aid='YDlc'
    set aid='YDld'
    set aid='YDle'
    set aid='YDlf'
    set aid='YDm0'
    set aid='YDm1'
    set aid='YDm2'
    set aid='YDm3'
    set aid='YDm4'
    set aid='YDm5'
    set aid='YDm6'
    set aid='YDm7'
    set aid='YDm8'
    set aid='YDm9'
    set aid='YDma'
    set aid='YDmb'
    set aid='YDmc'
    set aid='YDmd'
    set aid='YDme'
    set aid='YDmf'
    set aid='YDc0'
    set aid='YDc1'
    set aid='YDc2'
    set aid='YDc3'
    set aid='YDc4'
    set aid='YDc5'
    set aid='YDc6'
    set aid='YDc7'
    set aid='YDc8'
    set aid='YDc9'
    set aid='YDca'
    set aid='YDcb'
    set aid='YDcc'
    set aid='YDb0'
    set aid='YDb1'
    set aid='YDb2'
    set aid='YDb3'
    set aid='YDb4'
    set aid='YDb5'
    set aid='YDb6'
    set aid='YDb7'
    set aid='YDb8'
    set aid='YDb9'
    set aid='YDba'
    set aid='YDbb'
    set aid='YDbc'
    set aid='YDbd'
    set aid='YDbe'
    set aid='YDbf'
    set aid='YDbg'
    set aid='YDbh'
    set aid='YDbi'
    set aid='YDbj'
    set aid='YDbk'
    set aid='YDbl'
    set aid='YDbm'
    set aid='YDbn'
endfunction

    private function UnitClearBonus takes unit u, integer bonusType returns nothing
        local integer i=ABILITY_COUNT[bonusType+1]-1
        loop
            exitwhen i < ABILITY_COUNT[bonusType]
            call UnitRemoveAbility(u,BonusAbilitys[i])
            set i=i-1
        endloop
        call YDWESaveIntegerByString(I2S(YDWEH2I(u)),"bonusType"+I2S(bonusType),0)
    endfunction

    private function SetUnitMaxState takes unit u,integer bonusType,real value returns boolean
        local integer v=R2I(value)
        local integer s=0
        local integer o=0
        local unitstate state
        local integer i=ABILITY_COUNT[bonusType+1]- 2
        local integer a=ABILITY_NUM[bonusType]
        if value>65535 or value<=0 then
            debug call BJDebugMsg("输入数据无效")
            return false
        endif

        if bonusType == 0 then
            set state=UNIT_STATE_MAX_LIFE
          elseif bonusType == 1 then
            set state=UNIT_STATE_MAX_MANA
          else
            debug call BJDebugMsg("无效状态")
            return false
        endif
        set v=v-R2I(GetUnitState(u, state))
        call YDWESaveIntegerByString(I2S(YDWEH2I(u)),"bonusType"+I2S(bonusType),v)
        if v>0 then
            set o=3
          elseif v<0 then
            set o=2
            set v=-v
          else
            return false
        endif
        loop
            exitwhen i < ABILITY_COUNT[bonusType]
            if v >= PowersOf2[i] then
                call UnitAddAbility(u, BonusAbilitys[i])
                call SetUnitAbilityLevel(u, BonusAbilitys[i], o)
                call UnitRemoveAbility(u, BonusAbilitys[i])
                set v=v-PowersOf2[i]
            endif
            set i=i-1
        endloop
        return true
    endfunction

private function UnitSetBonus takes unit u, integer bonusType, integer ammount returns boolean
    local integer i
    //设置属性为0不进行Loop
    if  ammount==0 then
        call UnitClearBonus(u,bonusType)
        return false
    endif
    if ammount < MinBonus[bonusType] or ammount > MaxBonus[bonusType] then
        debug call BJDebugMsg("BonusSystem Error: Bonus too high or low (" + I2S(ammount) + ")")
        return false
    elseif bonusType < 0 or bonusType >= BONUS_TYPES then
        debug call BJDebugMsg("BonusSystem Error: Invalid bonus type (" + I2S(bonusType) + ")")
        return false
    endif

    call YDWESaveIntegerByString(I2S(YDWEH2I(u)),"bonusType"+I2S(bonusType),ammount)
    if ammount < 0 then
        set ammount = MaxBonus[bonusType] + ammount + 1
        call UnitAddAbility(u, BonusAbilitys[ABILITY_COUNT[bonusType+1]  - 1])
        call UnitMakeAbilityPermanent(u, true, BonusAbilitys[ABILITY_COUNT[bonusType+1]  - 1])
      else
        call UnitRemoveAbility(u,BonusAbilitys[ABILITY_COUNT[bonusType+1]  - 1])
    endif

    set i = ABILITY_COUNT[bonusType+1]-2
    loop
        exitwhen i < ABILITY_COUNT[bonusType]
        if ammount >= PowersOf2[i] then
            call UnitAddAbility(u,BonusAbilitys[i])
            call UnitMakeAbilityPermanent(u, true, BonusAbilitys[i])
            set ammount = ammount - PowersOf2[i]
        else
            call UnitRemoveAbility(u,BonusAbilitys[i])
        endif

        set i = i - 1
    endloop

    if not YDWEGetBooleanByString(I2S(YDWEH2I(u)),"bonusMark") then
        call YDWESaveBooleanByString(I2S(YDWEH2I(u)),"bonusMark", true)
        set UnitCount = UnitCount + 1
        set Units[UnitCount] = u
    endif

    return true
endfunction

private function UnitGetBonus takes unit u, integer bonusType returns integer
    return YDWEGetIntegerByString(I2S(YDWEH2I(u)),"bonusType"+I2S(bonusType))
endfunction

private function AddUnitMaxState takes unit u,integer bonusType,real value returns boolean
    local unitstate state
    if bonusType==0 then
        set state=UNIT_STATE_MAX_LIFE
      elseif bonusType==1 then
        set state=UNIT_STATE_MAX_MANA
      else
        return false
    endif
    return SetUnitMaxState( u,bonusType, value+GetUnitState(u,state))
endfunction

private function UnitAddBonus takes unit u, integer bonusType, integer ammount returns boolean
    return UnitSetBonus(u, bonusType, UnitGetBonus(u, bonusType) + ammount)
endfunction

private function FlushUnits takes nothing returns nothing
    local integer i = UnitCount
    local string h
    loop
        exitwhen i < 1
        if GetUnitTypeId(Units[i]) == 0 then
            set h = I2S(YDWEH2I(Units[i]))
            call YDWESaveIntegerByString(h,"bonusType0",0)
            call YDWESaveIntegerByString(h,"bonusType1",0)
            call YDWESaveIntegerByString(h,"bonusType2",0)
            call YDWESaveIntegerByString(h,"bonusType3",0)
            call YDWESaveBooleanByString(h,"bonusMark",false)
            set Units[i] = Units[UnitCount]
            set Units[UnitCount] = null
            set UnitCount = UnitCount - 1
        endif
        set i = i - 1
    endloop
endfunction

function YDWEUnitSetBonus takes unit u, integer bonusType, integer ammount returns nothing
    if bonusType==0 or bonusType==1 then
        call SetUnitMaxState(u,bonusType,ammount)
      else
        call UnitSetBonus(u,bonusType,ammount)
    endif
endfunction

function YDWEUnitAddBonus takes unit u, integer bonusType, integer ammount returns nothing
    if bonusType==0 or bonusType==1 then
        call AddUnitMaxState(u,bonusType,ammount)
      else
        call UnitAddBonus(u,bonusType,ammount)
    endif
endfunction

function YDWEGeneralBounsSystemUnitSetBonus takes unit u,integer bonusType,integer mod,integer ammount returns nothing
    if mod==0 then
        call YDWEUnitAddBonus(u,bonusType,ammount)
      elseif mod==1 then
        call YDWEUnitAddBonus(u,bonusType,0-ammount)
      else
        call YDWEUnitSetBonus(u,bonusType,ammount)
    endif
endfunction

function YDWEGeneralBounsSystemUnitGetBonus takes unit u,integer bonusType returns integer
    return YDWEGetIntegerByString(I2S(YDWEH2I(u)),"bonusType"+I2S(bonusType))
endfunction

//private keyword BonusAbilitys
private function InitializeAbilitys takes nothing returns nothing
    local integer i=0
    local integer m=0

    set ABILITY_COUNT[0]=0  //life max
    set ABILITY_COUNT[1]=15 //mana max
    set ABILITY_COUNT[2]=30 //armor
    set ABILITY_COUNT[3]=43 //attack
    set ABILITY_COUNT[4]=67

    set ABILITY_NUM[0]=15
    set ABILITY_NUM[1]=15
    set ABILITY_NUM[2]=13
    set ABILITY_NUM[3]=24

    // Bonus Mod - armor abilitys
    loop
        exitwhen i>9
        set YDWEBONUS_MyChar[i]='0'+m
        set m=m+1
        set i=i+1
    endloop
    set m=0
    loop
        exitwhen i>26
        set YDWEBONUS_MyChar[i]='a'+m
        set m=m+1
        set i=i+1
    endloop

    set i =0
    set m=0
    loop
        exitwhen m>(ABILITY_NUM[0]-1)
        set BonusAbilitys[i] = 'YDl0' -'0'+YDWEBONUS_MyChar[m]// +1
        set i=i+1
        set m=m+1
    endloop
    // Bonus Mod - Damage abilitys
    set m=0
    loop
        exitwhen m>(ABILITY_NUM[1]-1)
        set BonusAbilitys[i] = 'YDm0' -'0'+YDWEBONUS_MyChar[m]// +1
        set i=i+1
        set m=m+1
    endloop
    // Bonus Mod - Life MaxState abilitys
    set m=0
    loop
        exitwhen m>(ABILITY_NUM[2]-1)
        set BonusAbilitys[i] = 'YDc0' -'0'+YDWEBONUS_MyChar[m]// +1
        set i=i+1
        set m=m+1
    endloop
    // Bonus Mod - Mana MaxState abilitys
    set m=0
    loop
        exitwhen m>(ABILITY_NUM[3]-1)
        set BonusAbilitys[i] = 'YDb0' -'0'+YDWEBONUS_MyChar[m]// +1
        set i=i+1
        set m=m+1
    endloop
endfunction

private function Initialize takes nothing returns nothing
    local integer i = 1
    local unit u
    local integer n=0
    local integer add=0
    call InitializeAbilitys()
    loop
        set i=1
        set PowersOf2[add] = 1
            loop
                set PowersOf2[add+1] = PowersOf2[add ] * 2
                set add=add+1
                set i = i + 1
                exitwhen i == ABILITY_NUM[n]
            endloop
        set MaxBonus[n] = PowersOf2[add] - 1
        set MinBonus[n] = -PowersOf2[add]
        set add=add+1
        set n=n+1
        exitwhen n>=4
    endloop
    //预读技能
    if PRELOAD_ABILITYS then
        set u = CreateUnit(Player(15), PRELOAD_DUMMY_UNIT, 0, 0, 0)
        set i = 0
        loop
            exitwhen i == ABILITY_COUNT[5]
            call UnitAddAbility(u, BonusAbilitys[i])
            set i = i + 1
        endloop
        call RemoveUnit(u)
        set u = null
    endif
    //回收数据
    call TimerStart(CreateTimer(), 10, true, function FlushUnits)
endfunction

function YDWELifeChange takes unit u,integer mod,integer ch,integer id returns nothing
    local integer a
    local integer b
    local integer c
    local integer d
    local integer aid = id

    if mod==1 then
        set ch=-ch
      elseif mod==2 then
        set ch=ch-R2I(GetUnitState(u,UNIT_STATE_MAX_LIFE))
    endif
    //set YDWEADDBONUS_LIFE=YDWEADDBONUS_LIFE+ch
    if ch>99999 then
        set ch=99999
    endif
    if ch<-99999 then
        set ch=-99999
    endif
    if ch<0 then
        set a=2
        set ch=-ch
    else
        set a=7
    endif
    set b=0
    loop
        exitwhen b==5
        set c=ch-ch/10*10
        set d=0
        loop
            exitwhen d==c
            call UnitAddAbility(u,aid)
            call SetUnitAbilityLevel(u,aid,a)
            call UnitRemoveAbility(u,aid)
            set d=d+1
        endloop
        set ch=ch/10
        set a=a+1
        set b=b+1
    endloop

endfunction

//===========================================================================
//修改魔法
//===========================================================================
function YDWEManaChange takes unit u,integer mod,integer ch,integer id returns nothing
    local integer a
    local integer b
    local integer c
    local integer d
    local integer aid = id

    if mod==1 then
        set ch=-ch
      elseif mod==2 then
        set ch=ch-R2I(GetUnitState(u,UNIT_STATE_MAX_MANA))
    endif
    //set YDWEADDBONUS_MANA=YDWEADDBONUS_MANA+ch
    if ch>99999 then
        set ch=99999
    endif
    if ch<-99999 then
        set ch=-99999
    endif
    if ch<0 then
        set a=2
        set ch=-ch
    else
        set a=7
    endif

    set b=0
    loop
        exitwhen b==5
        set c=ch-ch/10*10
        set d=0
        loop
            exitwhen d==c
            call UnitAddAbility(u,aid)
            call SetUnitAbilityLevel(u,aid,a)
            call UnitRemoveAbility(u,aid)
            set d=d+1
        endloop
        set ch=ch/10
        set a=a+1
        set b=b+1
    endloop
endfunction

endlibrary

#endif /// YDWEGeneralBounsSystemIncluded
