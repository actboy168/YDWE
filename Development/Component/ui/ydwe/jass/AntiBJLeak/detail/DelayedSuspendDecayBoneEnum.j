#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEDelayedSuspendDecayBoneEnumNullIncluded
#define YDWEDelayedSuspendDecayBoneEnumNullIncluded


library YDWEDelayedSuspendDecayBoneEnumNull

function YDWEDelayedSuspendDecayBoneEnumNull takes nothing returns nothing
    local unit enumUnit = GetEnumUnit()

    if (GetUnitState(enumUnit, UNIT_STATE_LIFE) <= 0) then
        call UnitSuspendDecay(enumUnit, true)
        call SetUnitTimeScale(enumUnit, 0.0001)
    endif
    set enumUnit = null
endfunction

endlibrary

#endif
#endif
