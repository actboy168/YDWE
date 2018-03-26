#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetUnitsInRangeOfLocAllNullIncluded
#define YDWEGetUnitsInRangeOfLocAllNullIncluded

#include "AntiBJLeak/detail/GetUnitsInRangeOfLocMatching.j"

library YDWEGetUnitsInRangeOfLocAllNull requires YDWEGetUnitsInRangeOfLocMatchingNull

function YDWEGetUnitsInRangeOfLocAllNull takes real radius, location whichLocation returns group
    return YDWEGetUnitsInRangeOfLocMatchingNull(radius, whichLocation, null)
endfunction

endlibrary

#endif
#endif
