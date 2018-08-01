#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEEnumDestructablesInCircleBJNullIncluded
#define YDWEEnumDestructablesInCircleBJNullIncluded

#include "AntiBJLeak/detail/EnumDestructablesInCircleBJFilter.j"

library YDWEEnumDestructablesInCircleBJNull requires YDWEEnumDestructablesInCircleBJFilterNull

function YDWEEnumDestructablesInCircleBJNull takes real radius, location loc, code actionFunc returns nothing
    local rect r
    local real centerX = GetLocationX(loc)
    local real centerY = GetLocationY(loc)
    if radius >= 0 then
        set bj_enumDestructableCenter = loc
        set bj_enumDestructableRadius = radius
        set r = Rect(centerX - radius, centerY - radius, centerX + radius, centerY + radius)
        call EnumDestructablesInRect(r, Filter(function YDWEEnumDestructablesInCircleBJFilterNull), actionFunc)
        call RemoveRect(r)
    	set r = null
    endif
endfunction

endlibrary

#endif
#endif
