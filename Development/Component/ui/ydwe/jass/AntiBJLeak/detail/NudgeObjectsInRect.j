#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWENudgeObjectsInRectNullIncluded
#define YDWENudgeObjectsInRectNullIncluded

#include "AntiBJLeak/detail/NudgeUnitsInRectEnum.j"
#include "AntiBJLeak/detail/NudgeItemsInRectEnum.j"

library YDWENudgeObjectsInRectNull requires YDWENudgeUnitsInRectEnumNull, YDWENudgeItemsInRectEnumNull

function YDWENudgeObjectsInRectNull takes rect nudgeArea returns nothing
    local group g = CreateGroup()
    call GroupEnumUnitsInRect(g, nudgeArea, null)
    call ForGroup(g, function YDWENudgeUnitsInRectEnumNull)
    call DestroyGroup(g)
	set g = null

    call EnumItemsInRect(nudgeArea, null, function YDWENudgeItemsInRectEnumNull)
endfunction

endlibrary

#endif
#endif
