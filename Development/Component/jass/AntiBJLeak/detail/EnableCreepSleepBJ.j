#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEEnableCreepSleepBJNullIncluded
#define YDWEEnableCreepSleepBJNullIncluded

#include "AntiBJLeak/detail/WakePlayerUnits.j"

library YDWEEnableCreepSleepBJNull requires YDWEWakePlayerUnitsNull

function YDWEEnableCreepSleepBJNull takes boolean enable returns nothing
    call SetPlayerState(Player(PLAYER_NEUTRAL_AGGRESSIVE), PLAYER_STATE_NO_CREEP_SLEEP, IntegerTertiaryOp(enable, 0, 1))

    // If we're disabling, attempt to wake any already-sleeping creeps.
    if (not enable) then
        call YDWEWakePlayerUnitsNull(Player(PLAYER_NEUTRAL_AGGRESSIVE))
    endif
endfunction

endlibrary

#endif
#endif
