#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEInitRescuableBehaviorBJNullIncluded
#define YDWEInitRescuableBehaviorBJNullIncluded

#include "AntiBJLeak/detail/TryInitRescuableTriggersBJ.j"

library YDWEInitRescuableBehaviorBJNull requires YDWETryInitRescuableTriggersBJNull

function YDWEInitRescuableBehaviorBJNull takes nothing returns nothing
    local integer index

    set index = 0
    loop
        // If at least one player slot is "Rescuable"-controlled, init the
        // rescue behavior triggers.
        if (GetPlayerController(Player(index)) == MAP_CONTROL_RESCUABLE) then
            call YDWETryInitRescuableTriggersBJNull()
            return
        endif
        set index = index + 1
        exitwhen index == bj_MAX_PLAYERS
    endloop
endfunction

endlibrary

#endif
#endif
