# /*
#  *  引入YDTrigger存储系统 (需要VJass的支持)
#  *  
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_IMPORT_SAVE_LOAD_SYSTEM_H
#define INCLUDE_IMPORT_SAVE_LOAD_SYSTEM_H

#if WARCRAFT_VERSION >= 124
#    define YDHASH_HANDLE_TYPE hashtable
#    define YDHASH_HANDLE YDHT
#    define YDHASH_HANDLE_INIT(name) InitHashtable()
#else
#    define YDHASH_HANDLE_TYPE gamecache
#    define YDHASH_HANDLE YDGC
#    define YDHASH_HANDLE_INIT(name) InitGameCache(name)
#endif

library YDTriggerSaveLoadSystem initializer Init

#if WARCRAFT_VERSION >= 124
    #include <YDTrigger/SaveLoadSystem/HTSet_Get.h>
#else
    #include <YDTrigger/SaveLoadSystem/GCSet_Get.h>
#endif
    #include <YDTrigger/SaveLoadSystem/Any2I.h>
    #include <YDTrigger/SaveLoadSystem/Has_Clear.h>

globals
    #ifndef YDWE_HASH_DEFVAR
    #define YDWE_HASH_DEFVAR
        YDHASH_HANDLE_TYPE YDHASH_HANDLE
    #endif
    YDHASH_HANDLE_TYPE YDLOC
endglobals

#if WARCRAFT_VERSION < 124
    #include <YDTrigger/SaveLoadSystem/GCSystem.j>
#endif

    private function Init takes nothing returns nothing
        #ifndef YDWE_HASHTABLE_INITVAR
        #define YDWE_HASHTABLE_INITVAR
            set YDHASH_HANDLE = YDHASH_HANDLE_INIT("YDWE.wav")
        #endif
        set YDLOC = YDHASH_HANDLE_INIT("YDLOC")
    endfunction

endlibrary

#endif
