#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetUnitsInRectMatchingNullIncluded
#define YDWEGetUnitsInRectMatchingNullIncluded


library YDWEGetUnitsInRectMatchingNull

globals
#ifndef YDWE_NNULLTEMPGROUP_DEFVAR
#define YDWE_NNULLTEMPGROUP_DEFVAR
    group yd_NullTempGroup
#endif
endglobals

function YDWEGetUnitsInRectMatchingNull takes rect r, boolexpr filter returns group
    local group g = CreateGroup()
    call GroupEnumUnitsInRect(g, r, filter)
    call DestroyBoolExpr(filter)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

endlibrary

#endif
#endif
