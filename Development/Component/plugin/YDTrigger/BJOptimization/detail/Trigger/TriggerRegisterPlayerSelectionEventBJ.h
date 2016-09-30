# /*
#  *  TriggerRegisterPlayerSelectionEventBJ
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_TRIGGER_REGISTER_PLAYER_SELECTION_EVENT_BJ_H
#define INCLUDE_BJ_OPTIMIZATION_TRIGGER_REGISTER_PLAYER_SELECTION_EVENT_BJ_H
#
#  include <YDTrigger/Common/if.h>
#
#  define TriggerRegisterPlayerSelectionEventBJ(trig, player, selected)                  \
     YDTRIGGER_COMMON_IF(selected,                                                       \
       TriggerRegisterPlayerUnitEvent(trig, player, EVENT_PLAYER_UNIT_SELECTED, null),   \
       TriggerRegisterPlayerUnitEvent(trig, player, EVENT_PLAYER_UNIT_DESELECTED, null)  \
       )
#
#endif
