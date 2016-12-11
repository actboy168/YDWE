#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetUnitsInRectAllNullIncluded
#define YDWEGetUnitsInRectAllNullIncluded

#include "AntiBJLeak/detail/GetUnitsInRectMatching.j"

library YDWEGetUnitsInRectAllNull requires YDWEGetUnitsInRectMatchingNull

function YDWEGetUnitsInRectAllNull takes rect r returns group
    return YDWEGetUnitsInRectMatchingNull(r, null)
endfunction

endlibrary

#endif
#endif
