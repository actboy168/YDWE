#ifndef INCLUDE_SAVE_LOAD_SYSTEM_GC_JASS_H
#define INCLUDE_SAVE_LOAD_SYSTEM_GC_JASS_H

function YDTRIGGER_H2I takes handle h returns  integer
  return h
  return 0
endfunction

function YDTRIGGER_S2I takes string s returns  integer
  return s
  return 0
endfunction
     
#define YDTRIGGER_GET_PROC(type)                                                          YDNL \
  function YDTRIGGER_GET_##type takes gamecache gc, string table, string key returns type YDNL \
    return GetStoredInteger(gc, table, key)                                               YDNL \
    return null                                                                           YDNL \
  endfunction
     
YDTRIGGER_GET_PROC(timer)
YDTRIGGER_GET_PROC(trigger)
YDTRIGGER_GET_PROC(unit) 
YDTRIGGER_GET_PROC(item) 
YDTRIGGER_GET_PROC(group)
YDTRIGGER_GET_PROC(player) 
YDTRIGGER_GET_PROC(location)  
YDTRIGGER_GET_PROC(destructable)
YDTRIGGER_GET_PROC(force)
YDTRIGGER_GET_PROC(rect)
YDTRIGGER_GET_PROC(region)
YDTRIGGER_GET_PROC(sound)
YDTRIGGER_GET_PROC(effect) 
YDTRIGGER_GET_PROC(unitpool)
YDTRIGGER_GET_PROC(itempool)
YDTRIGGER_GET_PROC(quest)
YDTRIGGER_GET_PROC(questitem)
YDTRIGGER_GET_PROC(timerdialog)
YDTRIGGER_GET_PROC(leaderboard)
YDTRIGGER_GET_PROC(multiboard)
YDTRIGGER_GET_PROC(multiboarditem)
YDTRIGGER_GET_PROC(trackable)
YDTRIGGER_GET_PROC(dialog)
YDTRIGGER_GET_PROC(button)  
YDTRIGGER_GET_PROC(texttag)
YDTRIGGER_GET_PROC(lightning)
YDTRIGGER_GET_PROC(image)
YDTRIGGER_GET_PROC(fogstate)
YDTRIGGER_GET_PROC(fogmodifier)
 
#undef YDTRIGGER_GET_PROC

#endif
