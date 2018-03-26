#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetUnitsOfPlayerAndTypeIdNullIncluded
#define YDWEGetUnitsOfPlayerAndTypeIdNullIncluded


library YDWEGetUnitsOfPlayerAndTypeIdNull

globals
#ifndef YDWE_NNULLTEMPGROUP_DEFVAR
#define YDWE_NNULLTEMPGROUP_DEFVAR
    group yd_NullTempGroup
#endif
endglobals

function YDWEGetUnitsOfPlayerAndTypeIdNull takes player whichPlayer, integer unitid returns group
    local group g = CreateGroup()
    set bj_groupEnumTypeId = unitid
    call GroupEnumUnitsOfPlayer(g, whichPlayer, filterGetUnitsOfPlayerAndTypeId)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

endlibrary

#endif
#endif
