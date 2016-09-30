# /*
#  *  CheckItemStatus
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_CHECK_ITEM_STATUS_H
#define INCLUDE_BJ_OPTIMIZATION_CHECK_ITEM_STATUS_H
#
#  include <YDTrigger/Common/switch.h>
#
#  define CheckItemStatus(item, status)                     \
     YDTRIGGER_COMMON_SWITCH(CHECK_ITEM_STATUS_## status, ( \
       (not IsItemVisible(item)),                           \
       IsItemOwned(item),                                   \
       IsItemInvulnerable(item),                            \
       IsItemPowerup(item),                                 \
       IsItemSellable(item),                                \
       IsItemPawnable(item),                                \
       ))                                            
#
#  define CHECK_ITEM_STATUS_bj_ITEM_STATUS_HIDDEN        0
#  define CHECK_ITEM_STATUS_bj_ITEM_STATUS_OWNED         1
#  define CHECK_ITEM_STATUS_bj_ITEM_STATUS_INVULNERABLE  2
#  define CHECK_ITEM_STATUS_bj_ITEM_STATUS_POWERUP       3
#  define CHECK_ITEM_STATUS_bj_ITEM_STATUS_SELLABLE      4
#  define CHECK_ITEM_STATUS_bj_ITEM_STATUS_PAWNABLE      5
#
#endif
