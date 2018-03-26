#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWESetUnitFacingToFaceLocTimedNullIncluded
#define YDWESetUnitFacingToFaceLocTimedNullIncluded


library YDWESetUnitFacingToFaceLocTimedNull

function YDWESetUnitFacingToFaceLocTimedNull takes unit whichUnit, location target, real duration returns nothing
    local location unitLoc = GetUnitLoc(whichUnit)

    call SetUnitFacingTimed(whichUnit, AngleBetweenPoints(unitLoc, target), duration)
    call RemoveLocation(unitLoc)
    set unitLoc = null
endfunction

endlibrary

#endif
#endif
