#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEMakeUnitRescuableToForceBJNullIncluded
#define YDWEMakeUnitRescuableToForceBJNullIncluded

#include "AntiBJLeak/detail/MakeUnitRescuableToForceBJEnum.j"

library YDWEMakeUnitRescuableToForceBJNull requires YDWEMakeUnitRescuableToForceBJEnumNull

function YDWEMakeUnitRescuableToForceBJNull takes unit whichUnit, boolean isRescuable, force whichForce returns nothing
    // Flag the unit as rescuable/unrescuable for the appropriate players.
    set bj_makeUnitRescuableUnit = whichUnit
    set bj_makeUnitRescuableFlag = isRescuable
    call ForForce(whichForce, function YDWEMakeUnitRescuableToForceBJEnumNull)
endfunction

endlibrary

#endif
#endif
