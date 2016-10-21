#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEChangeElevatorWallBlockerNullIncluded
#define YDWEChangeElevatorWallBlockerNullIncluded

#include "AntiBJLeak/detail/NudgeObjectsInRect.j"

library YDWEChangeElevatorWallBlockerNull requires YDWENudgeObjectsInRectNull

function YDWEChangeElevatorWallBlockerNull takes real x, real y, real facing, boolean open returns nothing
    local destructable blocker = null
    local real         findThreshold = 32
    local real         nudgeLength   = 4.25 * bj_CELLWIDTH
    local real         nudgeWidth    = 1.25 * bj_CELLWIDTH
    local rect         r

    // Search for the pathing blocker within the general area.
    set r = Rect(x - findThreshold, y - findThreshold, x + findThreshold, y + findThreshold)
    set bj_elevatorWallBlocker = null
    call EnumDestructablesInRect(r, null, function FindElevatorWallBlockerEnum)
    call RemoveRect(r)
    set blocker = bj_elevatorWallBlocker

    // Ensure that the blocker exists.
    if (blocker == null) then
        set blocker = CreateDeadDestructable(bj_ELEVATOR_BLOCKER_CODE, x, y, facing, 1, 0)
    elseif (GetDestructableTypeId(blocker) != bj_ELEVATOR_BLOCKER_CODE) then
        // If a different destructible exists in the blocker's spot, ignore
        // the request.  (Two destructibles cannot occupy the same location
        // on the map, so we cannot create an elevator blocker here.)
        return
    endif

    if (open) then
        // Ensure that the blocker is dead.
        if (GetDestructableLife(blocker) > 0) then
            call KillDestructable(blocker)
        endif
    else
        // Ensure that the blocker is alive.
        if (GetDestructableLife(blocker) <= 0) then
            call DestructableRestoreLife(blocker, GetDestructableMaxLife(blocker), false)
        endif

        // Nudge any objects standing in the blocker's way.
        if (facing == 0) then
            set r = Rect(x - nudgeWidth/2, y - nudgeLength/2, x + nudgeWidth/2, y + nudgeLength/2)
            call YDWENudgeObjectsInRectNull(r)
            call RemoveRect(r)
        elseif (facing == 90) then
            set r = Rect(x - nudgeLength/2, y - nudgeWidth/2, x + nudgeLength/2, y + nudgeWidth/2)
            call YDWENudgeObjectsInRectNull(r)
            call RemoveRect(r)
        else
            // Unrecognized blocker angle - don't nudge anything.
        endif
    endif
    set blocker = null
endfunction

endlibrary

#endif
#endif
