#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWESetUnitFacingToFaceUnitTimedNullIncluded
#define YDWESetUnitFacingToFaceUnitTimedNullIncluded

#include "AntiBJLeak/detail/SetUnitFacingToFaceLocTimed.j"

library YDWESetUnitFacingToFaceUnitTimedNull requires YDWESetUnitFacingToFaceLocTimedNull

function YDWESetUnitFacingToFaceUnitTimedNull takes unit whichUnit, unit target, real duration returns nothing
    local location unitLoc = GetUnitLoc(target)

    call YDWESetUnitFacingToFaceLocTimedNull(whichUnit, unitLoc, duration)
    call RemoveLocation(unitLoc)
    set unitLoc = null
endfunction

endlibrary

#endif
#endif
