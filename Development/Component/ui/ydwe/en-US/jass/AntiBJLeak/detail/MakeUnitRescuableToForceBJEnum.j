#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEMakeUnitRescuableToForceBJEnumNullIncluded
#define YDWEMakeUnitRescuableToForceBJEnumNullIncluded

#include "AntiBJLeak/detail/TryInitRescuableTriggersBJ.j"

library YDWEMakeUnitRescuableToForceBJEnumNull requires YDWETryInitRescuableTriggersBJNull

function YDWEMakeUnitRescuableToForceBJEnumNull takes nothing returns nothing
    call YDWETryInitRescuableTriggersBJNull()
    call SetUnitRescuable(bj_makeUnitRescuableUnit, GetEnumPlayer(), bj_makeUnitRescuableFlag)
endfunction

endlibrary

#endif
#endif
