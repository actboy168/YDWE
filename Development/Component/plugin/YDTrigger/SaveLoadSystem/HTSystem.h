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
#    include <YDTrigger/SaveLoadSystem/HTSet_Get.h>
#    include <YDTrigger/SaveLoadSystem/Any2I.h>
#    include <YDTrigger/SaveLoadSystem/Has_Clear.h>
#
globals
#    ifndef YDWE_HASHTABLE_DEFVAR
#    define YDWE_HASHTABLE_DEFVAR
       hashtable YDHASH_HANDLE
#    endif
endglobals
#
	private function Init takes nothing returns nothing
#    ifndef YDWE_HASHTABLE_INITVAR
#    define YDWE_HASHTABLE_INITVAR
		set YDHASH_HANDLE = InitHashtable()    
#    endif
	endfunction
#
endlibrary

#endif
