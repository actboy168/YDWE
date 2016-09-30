# /*
#  *  UnitRemoveBuffsBJ
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_UNIT_REMOVE_BUFFS_H
#define INCLUDE_BJ_OPTIMIZATION_UNIT_REMOVE_BUFFS_H
#
#  include <YDTrigger/Common/switch.h>
#
#  define UnitRemoveBuffsBJ(buffType, unit)                                  \
     YDTRIGGER_COMMON_SWITCH(UNIT_REMOVE_BUFFS_## buffType, (                \
       UnitRemoveBuffs(unit, true, false),                                   \
       UnitRemoveBuffs(unit, false, true),                                   \
       UnitRemoveBuffs(unit, true, true),                                    \
       UnitRemoveBuffsEx(unit, true, true, false, false, false, true, false) \
       ))
#
#  define UNIT_REMOVE_BUFFS_bj_REMOVEBUFFS_POSITIVE 0
#  define UNIT_REMOVE_BUFFS_bj_REMOVEBUFFS_NEGATIVE 1
#  define UNIT_REMOVE_BUFFS_bj_REMOVEBUFFS_ALL      2
#  define UNIT_REMOVE_BUFFS_bj_REMOVEBUFFS_NONTLIFE 3
#
#endif

