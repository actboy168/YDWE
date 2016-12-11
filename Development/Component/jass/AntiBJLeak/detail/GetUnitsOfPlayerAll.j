#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetUnitsOfPlayerAllNullIncluded
#define YDWEGetUnitsOfPlayerAllNullIncluded

#include "AntiBJLeak/detail/GetUnitsOfPlayerMatching.j"

library YDWEGetUnitsOfPlayerAllNull requires YDWEGetUnitsOfPlayerMatchingNull

function YDWEGetUnitsOfPlayerAllNull takes player whichPlayer returns group
    return YDWEGetUnitsOfPlayerMatchingNull(whichPlayer, null)
endfunction

endlibrary

#endif
#endif
