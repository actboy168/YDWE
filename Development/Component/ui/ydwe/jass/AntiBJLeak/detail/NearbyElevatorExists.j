#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWENearbyElevatorExistsNullIncluded
#define YDWENearbyElevatorExistsNullIncluded

#include "AntiBJLeak/detail/NearbyElevatorExistsEnum.j"

library YDWENearbyElevatorExistsNull requires YDWENearbyElevatorExistsEnumNull

function YDWENearbyElevatorExistsNull takes real x, real y returns boolean
    local real findThreshold = 32
    local rect r

    // If another elevator is overlapping this one, ignore the wall.
    set r = Rect(x - findThreshold, y - findThreshold, x + findThreshold, y + findThreshold)
    set bj_elevatorNeighbor = null
    call EnumDestructablesInRect(r, null, function YDWENearbyElevatorExistsEnumNull)
    call RemoveRect(r)
    set r = null

    return bj_elevatorNeighbor != null
endfunction

endlibrary

#endif
#endif
