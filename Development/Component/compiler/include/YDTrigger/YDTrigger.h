# /*
#  *  YDTrigger专用宏
#  *  
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_H
#define INCLUDE_YDTRIGGER_H
#
#  ifndef DISABLE_SAVE_LOAD_SYSTEM
#      include <YDTrigger/ImportSaveLoadSystem.h>
#      include <YDTrigger/Hash.h>
#  endif
#
#  ifdef  USE_BJ_OPTIMIZATION 
#
#    include <YDTrigger/BJOptimization/Math.h>
#    include <YDTrigger/BJOptimization/General.h>
#    include <YDTrigger/BJOptimization/Camera.h>
#    include <YDTrigger/BJOptimization/Trigger.h>
#    include <YDTrigger/BJOptimization/Unit.h>
#    include <YDTrigger/BJOptimization/Animation.h>
#    include <YDTrigger/BJOptimization/Destructable.h>
#    include <YDTrigger/BJOptimization/Environment.h>
#    include <YDTrigger/BJOptimization/Group.h>
#    include <YDTrigger/BJOptimization/Hero.h>
#    include <YDTrigger/BJOptimization/Quest.h>
#    include <YDTrigger/BJOptimization/Sound.h>
#
#    ifdef USE_BJ_OPTIMIZATION_PRO
#      include <YDTrigger/BJOptimization/Pro_WidgetLift.h>
#    endif
#  endif
#
#  ifdef USE_BJ_ANTI_LEAK
#    include "AntiBJLeak/MainMacro.h"
#  endif
#
#  define YDWESaveTriggerName(t,s) DoNothing()
#
#  include <YDTrigger/Common/switch.h>
#  include <YDTrigger/Common/if.h>
#
#  define YDWEGetObjectProperty(object_type, object_id, property)            \
     YDTRIGGER_COMMON_SWITCH(YDWE_GET_OBJECT_PROPERTY_ ## object_type, (     \
       EXExecuteScript("(require'jass.slk').ability["      + I2S(object_id) + "]." + property), \
       EXExecuteScript("(require'jass.slk').buff["         + I2S(object_id) + "]." + property), \
       EXExecuteScript("(require'jass.slk').unit["         + I2S(object_id) + "]." + property), \
       EXExecuteScript("(require'jass.slk').item["         + I2S(object_id) + "]." + property), \
       EXExecuteScript("(require'jass.slk').upgrade["      + I2S(object_id) + "]." + property), \
       EXExecuteScript("(require'jass.slk').doodad["       + I2S(object_id) + "]." + property), \
       EXExecuteScript("(require'jass.slk').destructable[" + I2S(object_id) + "]." + property), \
       ))
#  define YDWEGetObjectPropertyInteger(type, id, prop) S2I(YDWEGetObjectProperty(type, id, prop))
#  define YDWEGetObjectPropertyReal(type, id, prop)    S2R(YDWEGetObjectProperty(type, id, prop))
#  define YDWEGetObjectPropertyString(type, id, prop)  (YDWEGetObjectProperty(type, id, prop))
#
#  define YDWE_GET_OBJECT_PROPERTY_YDWE_OBJECT_TYPE_ABILITY      0
#  define YDWE_GET_OBJECT_PROPERTY_YDWE_OBJECT_TYPE_BUFF         1
#  define YDWE_GET_OBJECT_PROPERTY_YDWE_OBJECT_TYPE_UNIT         2
#  define YDWE_GET_OBJECT_PROPERTY_YDWE_OBJECT_TYPE_ITEM         3
#  define YDWE_GET_OBJECT_PROPERTY_YDWE_OBJECT_TYPE_UPGRADE      4
#  define YDWE_GET_OBJECT_PROPERTY_YDWE_OBJECT_TYPE_DOODAD       5
#  define YDWE_GET_OBJECT_PROPERTY_YDWE_OBJECT_TYPE_DESTRUCTABLE 6
#
#  define YDTriggerExecuteTrigger(trig, check) \
        YDTRIGGER_COMMON_IF(check,\
            DoNothing() YDNL if TriggerEvaluate(trig) then YDNL call TriggerExecute(trig) YDNL endif, \
            TriggerExecute(trig) \
        )
#endif
