#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWETriggerActionUnitRescuedBJNullIncluded
#define YDWETriggerActionUnitRescuedBJNullIncluded


library YDWETriggerActionUnitRescuedBJNull

function YDWETriggerActionUnitRescuedBJNull takes nothing returns nothing
    local unit theUnit = GetTriggerUnit()

    if IsUnitType(theUnit, UNIT_TYPE_STRUCTURE) then
        call RescueUnitBJ(theUnit, GetOwningPlayer(GetRescuer()), bj_rescueChangeColorBldg)
    else
        call RescueUnitBJ(theUnit, GetOwningPlayer(GetRescuer()), bj_rescueChangeColorUnit)
    endif
    set theUnit = null
endfunction

endlibrary

#endif
#endif
