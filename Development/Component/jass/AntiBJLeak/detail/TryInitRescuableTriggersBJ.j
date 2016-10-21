#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWETryInitRescuableTriggersBJNullIncluded
#define YDWETryInitRescuableTriggersBJNullIncluded

#include "AntiBJLeak/detail/TriggerActionUnitRescuedBJ.j"

library YDWETryInitRescuableTriggersBJNull requires YDWETriggerActionUnitRescuedBJNull

function YDWETryInitRescuableTriggersBJNull takes nothing returns nothing
    local integer index

    if (bj_rescueUnitBehavior == null) then
        set bj_rescueUnitBehavior = CreateTrigger()
        set index = 0
        loop
            call TriggerRegisterPlayerUnitEvent(bj_rescueUnitBehavior, Player(index), EVENT_PLAYER_UNIT_RESCUED, null)
            set index = index + 1
            exitwhen index == bj_MAX_PLAYER_SLOTS
        endloop
        call TriggerAddAction(bj_rescueUnitBehavior, function YDWETriggerActionUnitRescuedBJNull)
    endif
endfunction

endlibrary

#endif
#endif
