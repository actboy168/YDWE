#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEUnitHasItemOfTypeBJNullIncluded
#define YDWEUnitHasItemOfTypeBJNullIncluded


library YDWEUnitHasItemOfTypeBJNull

function YDWEUnitHasItemOfTypeBJNull takes unit whichUnit, integer itemId returns boolean
    local integer index = 0

	if itemId != 0 then
		loop
			if GetItemTypeId(UnitItemInSlot(whichUnit, index)) == itemId then
				return true
			endif

			set index = index + 1
			exitwhen index >= bj_MAX_INVENTORY
		endloop
	endif
    return false
endfunction

endlibrary

#endif
#endif
