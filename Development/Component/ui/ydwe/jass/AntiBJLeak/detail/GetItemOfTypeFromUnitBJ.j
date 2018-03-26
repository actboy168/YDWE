#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetItemOfTypeFromUnitBJNullIncluded
#define YDWEGetItemOfTypeFromUnitBJNullIncluded


library YDWEGetItemOfTypeFromUnitBJNull

globals
#ifndef YDWE_NNULLTEMPITEM_DEFVAR
#define YDWE_NNULLTEMPITEM_DEFVAR
    item yd_NullTempItem
#endif
endglobals

function YDWEGetItemOfTypeFromUnitBJNull takes unit whichUnit, integer itemId returns item
    local integer index = 0
    loop
        set yd_NullTempItem = UnitItemInSlot(whichUnit, index)
        if GetItemTypeId(yd_NullTempItem) == itemId then
            return yd_NullTempItem
        endif

        set index = index + 1
        exitwhen index >= bj_MAX_INVENTORY
    endloop
    return null
endfunction

endlibrary

#endif
#endif
