# /*
#  *  TriggerRegisterPlayerKeyEventBJ
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_TRIGGER_REGISTER_PLAYER_KEY_EVENT_BJ_H
#define INCLUDE_BJ_OPTIMIZATION_TRIGGER_REGISTER_PLAYER_KEY_EVENT_BJ_H
#
#  include <YDTrigger/Common/switch.h>
#
#  define TriggerRegisterPlayerKeyEventBJ(trig, player, keType, keKey)          \
     YDTRIGGER_COMMON_SWITCH(TRIGGER_EVENT_TYPE_## keType ## _ ## keKey, (      \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_LEFT_DOWN),  \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_RIGHT_DOWN), \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_DOWN_DOWN),  \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_UP_DOWN),    \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_LEFT_UP),    \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_RIGHT_UP),   \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_DOWN_UP),    \
       TriggerRegisterPlayerEvent(trig, player, EVENT_PLAYER_ARROW_UP_UP),      \
       ))
#
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_DEPRESS_bj_KEYEVENTKEY_LEFT     0
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_DEPRESS_bj_KEYEVENTKEY_RIGHT    1
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_DEPRESS_bj_KEYEVENTKEY_DOWN     2
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_DEPRESS_bj_KEYEVENTKEY_UP       3
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_RELEASE_bj_KEYEVENTKEY_LEFT     4
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_RELEASE_bj_KEYEVENTKEY_RIGHT    5
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_RELEASE_bj_KEYEVENTKEY_DOWN     6
#  define TRIGGER_EVENT_TYPE_bj_KEYEVENTTYPE_RELEASE_bj_KEYEVENTKEY_UP       7
#
#endif
