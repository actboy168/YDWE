#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEIssueBuildOrderByIdLocBJNullIncluded
#define YDWEIssueBuildOrderByIdLocBJNullIncluded

#include "AntiBJLeak/detail/IssueHauntOrderAtLocBJ.j"

library YDWEIssueBuildOrderByIdLocBJNull requires YDWEIssueHauntOrderAtLocBJNull

function YDWEIssueBuildOrderByIdLocBJNull takes unit whichPeon, integer unitId, location loc returns boolean
    if (unitId == 'ugol') then
        return YDWEIssueHauntOrderAtLocBJNull(whichPeon, loc)
    else
        return IssueBuildOrderById(whichPeon, unitId, GetLocationX(loc), GetLocationY(loc))
    endif
endfunction

endlibrary

#endif
#endif
