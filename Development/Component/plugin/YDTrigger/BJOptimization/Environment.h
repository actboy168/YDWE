# /*
#  *  BJ函数优化 -- Environment
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_ENVIRONMENT_H
#define INCLUDE_BJ_OPTIMIZATION_ENVIRONMENT_H
#                                                        
#  define RemoveWeatherEffectBJ(effect)                          RemoveWeatherEffect(effect)
#  define TerrainDeformationStopBJ(deformation, duration)        TerrainDeformStop(deformation, R2I((duration) * 1000))
#  define DestroyLightningBJ(bolt)                               DestroyLightning(bolt)
#  define GetLightningColorRBJ(bolt)                             GetLightningColorR(bolt)
#  define GetLightningColorGBJ(bolt)                             GetLightningColorG(bolt)
#  define GetLightningColorBBJ(bolt)                             GetLightningColorB(bolt)
#  define SetLightningColorBJ(bolt, r, g, b, a)                  SetLightningColor(bolt, r, g, b, a)
#  define GetAbilityEffectBJ(abilcode, t, index)                 GetAbilityEffectById(abilcode, t, index)
#  define GetAbilitySoundBJ(abilcode, t)                         GetAbilitySoundById(abilcode, t)
#  define SetWaterBaseColorBJ(red, green, blue, transparency)    SetWaterBaseColor(PercentTo255(red), PercentTo255(green), PercentTo255(blue), PercentTo255(100.0-(transparency)))
#  define FogEnableOn()                                          FogEnable(true)
#  define FogEnableOff()                                         FogEnable(false)
#  define FogMaskEnableOn()                                      FogMaskEnable(true)
#  define FogMaskEnableOff()                                     FogMaskEnable(false)
#  define UseTimeOfDayBJ(flag)                                   SuspendTimeOfDay(not (flag))
#  define SetTerrainFogExBJ(style, zstart, zend, d, r, g, b)     SetTerrainFogEx(style, zstart, zend, d, (r) * 0.01, (g) * 0.01, (b) * 0.01)
#  define ResetTerrainFogBJ()                                    ResetTerrainFog()
#  define SetDoodadAnimationRectBJ(animName, doodadID, r)        SetDoodadAnimationRect(r, doodadID, animName, false)
#  define AddUnitAnimationPropertiesBJ(add, p, unit)             AddUnitAnimationProperties(unit, p, add)
#  define ShowImageBJ(flag, image)                               ShowImage(image, flag)
#  define SetImageColorBJ(image, red, green, blue, alpha)        SetImageColor(image, PercentTo255(red), PercentTo255(green), PercentTo255(blue), PercentTo255(100.0-(alpha)))
#  define ShowUbersplatBJ(flag, splat)                           ShowUbersplat(splat, flag)
#
# /*
#  *  有返回值的函数, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define GetLastCreatedWeatherEffect()                          bj_lastCreatedWeatherEffect
#  define GetLastCreatedTerrainDeformation()                     bj_lastCreatedTerrainDeformation
#  define GetLastCreatedLightningBJ()                            bj_lastCreatedLightning
#  define GetLastCreatedFogModifier()                            bj_lastCreatedFogModifier
#  define GetLastCreatedImage()                                  bj_lastCreatedImage
#  define GetLastCreatedUbersplat()                              bj_lastCreatedUbersplat
#
#endif

