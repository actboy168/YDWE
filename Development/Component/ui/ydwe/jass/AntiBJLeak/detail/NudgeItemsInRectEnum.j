#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWENudgeItemsInRectEnumNullIncluded
#define YDWENudgeItemsInRectEnumNullIncluded


library YDWENudgeItemsInRectEnumNull

function YDWENudgeItemsInRectEnumNull takes nothing returns nothing
    local item nudgee = GetEnumItem()

    call SetItemPosition(nudgee, GetItemX(nudgee), GetItemY(nudgee))
    set nudgee = null
endfunction

endlibrary

#endif
#endif
