#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEGetInventoryIndexOfItemTypeBJNullIncluded
#define YDWEGetInventoryIndexOfItemTypeBJNullIncluded


library YDWEGetInventoryIndexOfItemTypeBJNull

function YDWEGetInventoryIndexOfItemTypeBJNull takes unit whichUnit, integer itemId returns integer
    local integer index = 0

	if itemId != 0 then
		loop
			if GetItemTypeId(UnitItemInSlot(whichUnit, index)) == itemId then
				return index + 1
			endif

			set index = index + 1
			exitwhen index >= bj_MAX_INVENTORY
		endloop
	endif
    return 0
endfunction

endlibrary

#endif
#endif
