# /*
#  *  引入YDTrigger存储系统 (需要VJass的支持)
#  *  
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_SAVE_LOAD_SYSTEM_H
#define INCLUDE_YDTRIGGER_SAVE_LOAD_SYSTEM_H

library YDTriggerSaveLoadSystem initializer Init
#
#    include <YDTrigger/SaveLoadSystem/GCSet_Get.h>
#    include <YDTrigger/SaveLoadSystem/Any2I.h>
#    include <YDTrigger/SaveLoadSystem/Has_Clear.h>
#
globals
#    ifndef YDWE_GAMECACHE_DEFVAR
#    define YDWE_GAMECACHE_DEFVAR
       gamecache YDTRIGGER_GAMECACHE 
#    endif
endglobals
#
#    include <YDTrigger/SaveLoadSystem/GCSystem.j>
#
	private function Init takes nothing returns nothing
#  	 ifndef YDWE_GAMECACHE_INITVAR
#  	 define YDWE_GAMECACHE_INITVAR
		set YDTRIGGER_GAMECACHE = InitGameCache("YDWE.wav")
#    endif
	endfunction
#
endlibrary

#endif
