#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetPlayersByMapControlNullIncluded
#define YDWEGetPlayersByMapControlNullIncluded


library YDWEGetPlayersByMapControlNull

globals
#ifndef YDWE_NNULLTEMPFORCE_DEFVAR
#define YDWE_NNULLTEMPFORCE_DEFVAR
    force yd_NullTempForce
#endif
endglobals

function YDWEGetPlayersByMapControlNull takes mapcontrol whichControl returns force
    local force f = CreateForce()
    local integer playerIndex
    local player  indexPlayer

    set playerIndex = 0
    loop
        set indexPlayer = Player(playerIndex)
        if GetPlayerController(indexPlayer) == whichControl then
            call ForceAddPlayer(f, indexPlayer)
        endif

        set playerIndex = playerIndex + 1
        exitwhen playerIndex == bj_MAX_PLAYER_SLOTS
    endloop
    set indexPlayer = null
    set yd_NullTempForce = f
    set f = null
    return yd_NullTempForce
endfunction

endlibrary

#endif
#endif
