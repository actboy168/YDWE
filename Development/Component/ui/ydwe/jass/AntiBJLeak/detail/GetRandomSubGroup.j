#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetRandomSubGroupNullIncluded
#define YDWEGetRandomSubGroupNullIncluded

#include "AntiBJLeak/detail/GetRandomSubGroupEnum.j"

library YDWEGetRandomSubGroupNull requires YDWEGetRandomSubGroupEnumNull

function YDWEGetRandomSubGroupNull takes integer count, group sourceGroup returns group
    set bj_randomSubGroupGroup = CreateGroup()
    set bj_randomSubGroupWant  = count
    set bj_randomSubGroupTotal = CountUnitsInGroup(sourceGroup)

    if (bj_randomSubGroupWant <= 0 or bj_randomSubGroupTotal <= 0) then
        return bj_randomSubGroupGroup
    endif

    call ForGroup(sourceGroup, function YDWEGetRandomSubGroupEnumNull)
    return bj_randomSubGroupGroup
endfunction

endlibrary

#endif
#endif
