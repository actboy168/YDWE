#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEEnumUnitsSelectedNullIncluded
#define YDWEEnumUnitsSelectedNullIncluded


library YDWEEnumUnitsSelectedNull

function YDWEEnumUnitsSelectedNull takes player whichPlayer, boolexpr enumFilter, code enumAction returns nothing
    local group g = CreateGroup()
    call SyncSelections()
    call GroupEnumUnitsSelected(g, whichPlayer, enumFilter)
    call DestroyBoolExpr(enumFilter)
    call ForGroup(g, enumAction)
    call DestroyGroup(g)
    set g = null
endfunction

endlibrary

#endif
#endif
