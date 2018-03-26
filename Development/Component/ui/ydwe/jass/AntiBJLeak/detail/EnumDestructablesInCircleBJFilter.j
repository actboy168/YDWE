#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEEnumDestructablesInCircleBJFilterNullIncluded
#define YDWEEnumDestructablesInCircleBJFilterNullIncluded


library YDWEEnumDestructablesInCircleBJFilterNull

function YDWEEnumDestructablesInCircleBJFilterNull takes nothing returns boolean
    local real dx = GetDestructableX(GetFilterDestructable()) - GetLocationX(bj_enumDestructableCenter)
    local real dy = GetDestructableY(GetFilterDestructable()) - GetLocationY(bj_enumDestructableCenter)
    return dx * dx + dy * dy <= bj_enumDestructableRadius * bj_enumDestructableRadius
endfunction

endlibrary

#endif
#endif
