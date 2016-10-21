#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWETriggerRegisterLeaveRectSimpleNullIncluded
#define YDWETriggerRegisterLeaveRectSimpleNullIncluded


library YDWETriggerRegisterLeaveRectSimpleNull

globals
#ifndef YDWE_NNULLTEMPREGION_DEFVAR
#define YDWE_NNULLTEMPREGION_DEFVAR
    region yd_NullTempRegion
#endif
endglobals

function YDWETriggerRegisterLeaveRectSimpleNull takes trigger trig, rect r returns event
    local region rectRegion = CreateRegion()
    call RegionAddRect(rectRegion, r)
    set yd_NullTempRegion = rectRegion
    set rectRegion = null
    return TriggerRegisterLeaveRegion(trig, yd_NullTempRegion, null)
endfunction

endlibrary

#endif
#endif
