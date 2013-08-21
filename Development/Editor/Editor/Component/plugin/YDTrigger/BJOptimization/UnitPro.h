# /*
#  *  BJ函数优化 -- Unit.Pro
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_UNIT_PRO_H
#define INCLUDE_BJ_OPTIMIZATION_UNIT_PRO_H
#
#  include <YDTrigger/BJOptimization/Unit.h>
#
# /*
#  *  1.GetUnitState(unit, UNIT_STATE_LIFE) 替换为 GetWidgetLife(unit)
#  *  2.死亡判断改用更为精确的数值
#  */
#
#  undef  GetUnitStateSwap
#  define GetUnitStateSwap(state, unit)                      \
     YDTRIGGER_COMMON_SWITCH(GET_UNIT_STATE_SWAP_## state, ( \
       GetWidgetLife(unit),                                  \
       GetUnitState(unit, UNIT_STATE_MAX_LIFE),              \
       GetUnitState(unit, UNIT_STATE_MANA),                  \
       GetUnitState(unit, UNIT_STATE_MAX_MANA),              \
       ))
#
#  define GET_UNIT_STATE_SWAP_UNIT_STATE_LIFE     0
#  define GET_UNIT_STATE_SWAP_UNIT_STATE_MAX_LIFE 1
#  define GET_UNIT_STATE_SWAP_UNIT_STATE_MANA     2
#  define GET_UNIT_STATE_SWAP_UNIT_STATE_MAX_MANA 3
#
#  undef  SetUnitLifeBJ
#  define SetUnitLifeBJ(unit, value)                             SetWidgetLife(unit, RMaxBJ(0, value))
#
#  undef  IsUnitDeadBJ
#  define IsUnitDeadBJ(unit)                                     (GetWidgetLife(unit) <= 0.405)
#
#  undef  IsUnitAliveBJ
#  define IsUnitAliveBJ(unit)                                    (GetWidgetLife(unit) >  0.405)
#
#endif
