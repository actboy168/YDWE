# /*
#  *  BJ函数优化 -- Destructable
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_DESTRUCTABLE_H
#define INCLUDE_BJ_OPTIMIZATION_DESTRUCTABLE_H
#
#  define ShowDestructableBJ(flag, d)                                      ShowDestructable(d, flag)
#  define SetDestructableInvulnerableBJ(d, flag)                           SetDestructableInvulnerable(d, flag)
#  define IsDestructableInvulnerableBJ(d, flag)                            IsDestructableInvulnerable(d)
#  define EnumDestructablesInRectAll(r, actionFunc)                        EnumDestructablesInRect(r, null, actionFunc)
#  define RandomDestructableInRectSimpleBJ(r)                              RandomDestructableInRectBJ(r, null)
#  define SetDestructableMaxLifeBJ(d, max)                                 SetDestructableMaxLife(d, max)
#
# /*
#  *  有返回值的函数, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define GetLastCreatedDestructable()                                     bj_lastCreatedDestructable
#  define IsDestructableDeadBJ(d)                                          (GetDestructableLife(d) <= 0)
#  define IsDestructableAliveBJ(d)                                         (GetDestructableLife(d) >  0)
#
#endif
