# /*
#  *  BJº¯ÊýÓÅ»¯ -- Animation
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_ANIMATION_H
#define INCLUDE_BJ_OPTIMIZATION_ANIMATION_H
#                                                        
#  define ResetUnitAnimation(unit)                               SetUnitAnimation(unit, "stand")
#  define SetUnitTimeScalePercent(unit, percentScale)            SetUnitTimeScale(unit, (percentScale) * 0.01)
#  define SetUnitScalePercent(unit, X, Y, Z)                     SetUnitScale(unit, (X) * 0.01, (Y) * 0.01, (Z) * 0.01)
#  define SetUnitVertexColorBJ(unit, red, green, blue, t)        SetUnitVertexColor(unit, PercentTo255(red), PercentTo255(green), PercentTo255(blue), PercentTo255(100.0-(t)))
#  define UnitAddIndicatorBJ(unit, red, green, blue, t)          AddIndicator(unit, PercentTo255(red), PercentTo255(green), PercentTo255(blue), PercentTo255(100.0-(t)))
#  define DestructableAddIndicatorBJ(d, red, green, blue, t)     AddIndicator(d, PercentTo255(red), PercentTo255(green), PercentTo255(blue), PercentTo255(100.0-(t)))
#  define ItemAddIndicatorBJ(item, red, green, blue, t)          AddIndicator(item, PercentTo255(red), PercentTo255(green), PercentTo255(blue), PercentTo255(100.0-(t)))
#  define QueueUnitAnimationBJ(unit, animation)                  QueueUnitAnimation(unit, animation)
#  define SetDestructableAnimationBJ(d, animation)               SetDestructableAnimation(d, animation)
#  define QueueDestructableAnimationBJ(d, animation)             QueueDestructableAnimation(d, animation)
#  define SetDestAnimationSpeedPercent(d, percentScale)          SetDestructableAnimationSpeed(d, (percentScale) * 0.01)
#
#endif
