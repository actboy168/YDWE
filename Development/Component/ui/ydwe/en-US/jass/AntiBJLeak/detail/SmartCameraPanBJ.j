#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWESmartCameraPanBJNullIncluded
#define YDWESmartCameraPanBJNullIncluded


library YDWESmartCameraPanBJNull

function YDWESmartCameraPanBJNull takes player whichPlayer, location loc, real duration returns nothing
    local real dist2
	local real dx
	local real dy
    if (GetLocalPlayer() == whichPlayer) then
        // Use only local code (no net traffic) within this block to avoid desyncs.
		set dx = GetCameraTargetPositionX() - GetLocationX(loc)
		set dy = GetCameraTargetPositionY() - GetLocationY(loc)
        set dist2 = dx * dx + dy * dy
        if (dist2 >= bj_SMARTPAN_TRESHOLD_SNAP*bj_SMARTPAN_TRESHOLD_SNAP) then
            // If the user is too far away, snap the camera.
            call PanCameraToTimed(GetLocationX(loc), GetLocationY(loc), 0)
        elseif (dist2 >= bj_SMARTPAN_TRESHOLD_PAN*bj_SMARTPAN_TRESHOLD_PAN) then
            // If the user is moderately close, pan the camera.
            call PanCameraToTimed(GetLocationX(loc), GetLocationY(loc), duration)
        else
            // User is close enough, so don't touch the camera.
        endif
    endif
endfunction

endlibrary

#endif
#endif
