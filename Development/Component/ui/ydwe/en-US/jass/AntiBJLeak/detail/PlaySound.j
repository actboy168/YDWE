#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEPlaySoundNullIncluded
#define YDWEPlaySoundNullIncluded


library YDWEPlaySoundNull

function YDWEPlaySoundNull takes string soundName returns nothing
    local sound soundHandle = CreateSound(soundName, false, false, true, 12700, 12700, "")
    call StartSound(soundHandle)
    call KillSoundWhenDone(soundHandle)
    set soundHandle = null
endfunction

endlibrary

#endif
#endif
