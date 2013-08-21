# /*
#  *  BJ函数优化 -- Camera
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_CAMERA_H
#define INCLUDE_BJ_OPTIMIZATION_CAMERA_H
#
#  define CameraSetupGetFieldSwap(field, setup)                  CameraSetupGetField(setup, field)
#  define GetCurrentCameraBoundsMapRectBJ()                      Rect(GetCameraBoundMinX(), GetCameraBoundMinY(), GetCameraBoundMaxX(), GetCameraBoundMaxY())
#  define GetEntireMapRect()                                     GetWorldBounds()
#  define CameraSetSmoothingFactorBJ(factor)                     CameraSetSmoothingFactor(factor)
#  define CameraResetSmoothingFactorBJ()                         CameraSetSmoothingFactor(0)
#
# /*
#  *  有返回值的函数, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define GetCameraBoundsMapRect()                               bj_mapInitialCameraBounds
#  define GetPlayableMapRect()                                   bj_mapInitialPlayableArea
#
#endif
