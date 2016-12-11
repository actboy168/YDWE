#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEMakeUnitsPassiveForTeamNullIncluded
#define YDWEMakeUnitsPassiveForTeamNullIncluded

#include "AntiBJLeak/detail/MakeUnitsPassiveForPlayer.j"

library YDWEMakeUnitsPassiveForTeamNull requires YDWEMakeUnitsPassiveForPlayerNull

function YDWEMakeUnitsPassiveForTeamNull takes player whichPlayer returns nothing
    local integer playerIndex
    local player  indexPlayer

    set playerIndex = 0
    loop
        set indexPlayer = Player(playerIndex)
        if PlayersAreCoAllied(whichPlayer, indexPlayer) then
            call YDWEMakeUnitsPassiveForPlayerNull(indexPlayer)
        endif

        set playerIndex = playerIndex + 1
        exitwhen playerIndex == bj_MAX_PLAYERS
    endloop
    set indexPlayer = null
endfunction

endlibrary

#endif
#endif
