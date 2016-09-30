# /*
#  *  CheckItemcodeStatus
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_CHECK_ITEMCODE_STATUS_H
#define INCLUDE_BJ_OPTIMIZATION_CHECK_ITEMCODE_STATUS_H
#
#  include <YDTrigger/Common/switch.h>
#
#  define CheckItemcodeStatus(itemId, status)                   \
     YDTRIGGER_COMMON_SWITCH(CHECK_ITEMCODE_STATUS_## status, ( \
       IsItemIdPowerup(itemId),                                 \
       IsItemIdSellable(itemId),                                \
       IsItemIdPawnable(itemId),                                \
       ))                                            
#
#  define CHECK_ITEMCODE_STATUS_bj_ITEMCODE_STATUS_POWERUP   0
#  define CHECK_ITEMCODE_STATUS_bj_ITEMCODE_STATUS_SELLABLE  1
#  define CHECK_ITEMCODE_STATUS_bj_ITEMCODE_STATUS_PAWNABLE  2
#
#endif
