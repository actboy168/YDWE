# /*
#  *  UnitCountBuffsExBJ
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_UNIT_COUNT_BUFFS_EX_H
#define INCLUDE_BJ_OPTIMIZATION_UNIT_COUNT_BUFFS_EX_H
#
#  include <YDTrigger/BJOptimization/detail/UnitBuffsCheck.h>
#
#  define UnitCountBuffsExBJ(polarity, resist, unit, bTLife, bAura) \
       UnitCountBuffsEx(unit,               \
         UNIT_BUFFS_EX_POS_CHECK(polarity), \
         UNIT_BUFFS_EX_NEG_CHECK(polarity), \
         UNIT_BUFFS_EX_MAG_CHECK(resist),   \
         UNIT_BUFFS_EX_PHY_CHECK(resist),   \
         bTLife, bAura, false)
#
#endif
