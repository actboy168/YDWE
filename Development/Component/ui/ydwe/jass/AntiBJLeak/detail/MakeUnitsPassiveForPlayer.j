#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEMakeUnitsPassiveForPlayerNullIncluded
#define YDWEMakeUnitsPassiveForPlayerNullIncluded


library YDWEMakeUnitsPassiveForPlayerNull

function YDWEMakeUnitsPassiveForPlayerNull takes player whichPlayer returns nothing
    local group   playerUnits = CreateGroup()
    call CachePlayerHeroData(whichPlayer)
    call GroupEnumUnitsOfPlayer(playerUnits, whichPlayer, null)
    call ForGroup(playerUnits, function MakeUnitsPassiveForPlayerEnum)
    call DestroyGroup(playerUnits)
    set playerUnits = null
endfunction

endlibrary

#endif
#endif
