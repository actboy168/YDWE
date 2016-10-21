#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEWakePlayerUnitsNullIncluded
#define YDWEWakePlayerUnitsNullIncluded


library YDWEWakePlayerUnitsNull

function YDWEWakePlayerUnitsNull takes player whichPlayer returns nothing
    local group g = CreateGroup()
    call GroupEnumUnitsOfPlayer(g, whichPlayer, null)
    call ForGroup(g, function WakePlayerUnitsEnum)
    call DestroyGroup(g)
    set g = null
endfunction

endlibrary

#endif
#endif
