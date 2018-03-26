#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetPlayersEnemiesNullIncluded
#define YDWEGetPlayersEnemiesNullIncluded


library YDWEGetPlayersEnemiesNull

globals
#ifndef YDWE_NNULLTEMPFORCE_DEFVAR
#define YDWE_NNULLTEMPFORCE_DEFVAR
    force yd_NullTempForce
#endif
endglobals

function YDWEGetPlayersEnemiesNull takes player whichPlayer returns force
    local force f = CreateForce()
    call ForceEnumEnemies(f, whichPlayer, null)
    set yd_NullTempForce = f
    set f = null
    return yd_NullTempForce
endfunction

endlibrary

#endif
#endif
