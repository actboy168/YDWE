#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEBlightGoldMineForPlayerNullIncluded
#define YDWEBlightGoldMineForPlayerNullIncluded

#include "AntiBJLeak/detail/BlightGoldMineForPlayerBJ.j"

library YDWEBlightGoldMineForPlayerNull requires YDWEBlightGoldMineForPlayerBJNull

function YDWEBlightGoldMineForPlayerNull takes unit goldMine, player whichPlayer returns unit
    set bj_lastHauntedGoldMine = YDWEBlightGoldMineForPlayerBJNull(goldMine, whichPlayer)
    return bj_lastHauntedGoldMine
endfunction

endlibrary

#endif
#endif
