#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetUnitsSelectedAllNullIncluded
#define YDWEGetUnitsSelectedAllNullIncluded


library YDWEGetUnitsSelectedAllNull

globals
#ifndef YDWE_NNULLTEMPGROUP_DEFVAR
#define YDWE_NNULLTEMPGROUP_DEFVAR
    group yd_NullTempGroup
#endif
endglobals

function YDWEGetUnitsSelectedAllNull takes player whichPlayer returns group
    local group g = CreateGroup()
    call SyncSelections()
    call GroupEnumUnitsSelected(g, whichPlayer, null)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

endlibrary

#endif
#endif
