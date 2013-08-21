# /*
#  *  BJ函数优化 -- Sound
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_SOUND_H
#define INCLUDE_BJ_OPTIMIZATION_SOUND_H
#                                                        
#  define StopSoundBJ(soundHandle, fadeOut)                      StopSound(soundHandle, false, fadeOut)
#  define SetSoundVolumeBJ(soundHandle, volumePercent)           SetSoundVolume(soundHandle, PercentToInt(volumePercent, 127))
#  define SetSoundOffsetBJ(newOffset, soundHandle)               SetSoundPlayPosition(soundHandle, R2I((newOffset) * 1000))
#  define SetSoundDistanceCutoffBJ(soundHandle, cutoff)          SetSoundDistanceCutoff(soundHandle, cutoff)
#  define SetSoundPitchBJ(soundHandle, pitch)                    SetSoundPitch(soundHandle, pitch)
#  define AttachSoundToUnitBJ(soundHandle, unit)                 AttachSoundToUnit(soundHandle, unit)
#  define SetSoundConeAnglesBJ(sound, inside, outside, volume)   SetSoundConeAngles(sound, inside, outside, PercentToInt(volume, 127))
#  define KillSoundWhenDoneBJ(soundHandle)                       KillSoundWhenDone(soundHandle)
#  define PlayMusicBJ(musicFileName)      \
            PlayMusic(musicFileName) YDNL \
            set bj_lastPlayedMusic = musicFileName
#  define PlayMusicExBJ(musicFileName, startingOffset, fadeInTime)                                  \
            PlayMusicEx(musicFileName, R2I((startingOffset) * 1000), R2I((fadeInTime) * 1000)) YDNL \
            set bj_lastPlayedMusic = musicFileName
#  define PlayThematicMusicBJ(musicName)                         PlayThematicMusic(musicName)
#  define PlayThematicMusicExBJ(musicName, startingOffset)       PlayThematicMusicEx(musicName, R2I((startingOffset) * 1000))
#  define SetThematicMusicOffsetBJ(newOffset)                    SetThematicMusicOffset(R2I((newOffset) * 1000))
#  define EndThematicMusicBJ()                                   EndThematicMusic()
#  define StopMusicBJ(fadeOut)                                   StopMusic(fadeOut)
#  define ResumeMusicBJ()                                        ResumeMusic()
#  define SetMusicVolumeBJ(volumePercent)                        SetMusicVolume(PercentToInt(volumePercent, 127))
#  define VolumeGroupSetVolumeBJ(vgroup, percent)                VolumeGroupSetVolume(vgroup, (percent) * 0.01)
#  define VolumeGroupResetImmediateBJ()                          VolumeGroupReset()
#  define WaitForSoundBJ(soundHandle, offset)                    TriggerWaitForSound(soundHandle, offset)
#  define SetMapMusicIndexedBJ(musicName, index)                 SetMapMusic(musicName, false, index)
#  define SetMapMusicRandomBJ(musicName)                         SetMapMusic(musicName, true, 0)
#  define ClearMapMusicBJ()                                      ClearMapMusic()
#
# /*
#  *  有返回值的函数, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define GetSoundFileDurationBJ(musicFileName)                   (I2R(GetSoundFileDuration(musicFileName)) * 0.001)
#  define GetLastPlayedSound()                                    bj_lastPlayedSound
#  define GetLastPlayedMusic()                                    bj_lastPlayedMusic
#  define IsDawnDuskEnabled()                                     bj_useDawnDuskSounds
#
#endif
