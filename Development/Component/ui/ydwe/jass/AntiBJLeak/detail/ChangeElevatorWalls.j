#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEChangeElevatorWallsNullIncluded
#define YDWEChangeElevatorWallsNullIncluded

#include "AntiBJLeak/detail/ChangeElevatorWallBlocker.j"
#include "AntiBJLeak/detail/NearbyElevatorExists.j"

library YDWEChangeElevatorWallsNull requires YDWEChangeElevatorWallBlockerNull, YDWENearbyElevatorExistsNull

function YDWEChangeElevatorWallsNull takes boolean open, integer walls, destructable d returns nothing
    local real x = GetDestructableX(d)
    local real y = GetDestructableY(d)
    local real distToBlocker = 192
    local real distToNeighbor = 256

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_EAST) then
        if (not YDWENearbyElevatorExistsNull(x + distToNeighbor, y)) then
            call YDWEChangeElevatorWallBlockerNull(x + distToBlocker, y, 0, open)
        endif
    endif

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_NORTH) then
        if (not YDWENearbyElevatorExistsNull(x, y + distToNeighbor)) then
            call YDWEChangeElevatorWallBlockerNull(x, y + distToBlocker, 90, open)
        endif
    endif

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_SOUTH) then
        if (not YDWENearbyElevatorExistsNull(x, y - distToNeighbor)) then
            call YDWEChangeElevatorWallBlockerNull(x, y - distToBlocker, 90, open)
        endif
    endif

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_WEST) then
        if (not YDWENearbyElevatorExistsNull(x - distToNeighbor, y)) then
            call YDWEChangeElevatorWallBlockerNull(x - distToBlocker, y, 0, open)
        endif
    endif
endfunction

endlibrary

#endif
#endif
