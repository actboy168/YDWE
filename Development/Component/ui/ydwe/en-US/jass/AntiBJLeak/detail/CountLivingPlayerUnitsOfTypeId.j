#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWECountLivingPlayerUnitsOfTypeIdNullIncluded
#define YDWECountLivingPlayerUnitsOfTypeIdNullIncluded


library YDWECountLivingPlayerUnitsOfTypeIdNull

function YDWECountLivingPlayerUnitsOfTypeIdNull takes integer unitId, player whichPlayer returns integer
    local group g
    local integer matchedCount

    set g = CreateGroup()
    set bj_livingPlayerUnitsTypeId = unitId
    call GroupEnumUnitsOfPlayer(g, whichPlayer, filterLivingPlayerUnitsOfTypeId)
    set matchedCount = CountUnitsInGroup(g)
    call DestroyGroup(g)
    set g = null

    return matchedCount
endfunction

endlibrary

#endif
#endif
