# /*
#  *  引入YDTrigger存储系统 (需要VJass的支持)
#  *  
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_IMPORT_SAVE_LOAD_SYSTEM_H
#define INCLUDE_IMPORT_SAVE_LOAD_SYSTEM_H
#
#    if WARCRAFT_VERSION >= 124
#      include <YDTrigger/SaveLoadSystem/HTSystem.h>
#    else
#      include <YDTrigger/SaveLoadSystem/GCSystem.h>
#    endif
#
#endif
