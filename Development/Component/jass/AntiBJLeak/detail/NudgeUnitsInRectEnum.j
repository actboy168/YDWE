#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWENudgeUnitsInRectEnumNullIncluded
#define YDWENudgeUnitsInRectEnumNullIncluded


library YDWENudgeUnitsInRectEnumNull

function YDWENudgeUnitsInRectEnumNull takes nothing returns nothing
    local unit nudgee = GetEnumUnit()

    call SetUnitPosition(nudgee, GetUnitX(nudgee), GetUnitY(nudgee))
    set nudgee = null
endfunction

endlibrary

#endif
#endif
