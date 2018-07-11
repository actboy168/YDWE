#ifndef INCLUDE_SAVE_LOAD_SYSTEM_HT_JASS_H
#define INCLUDE_SAVE_LOAD_SYSTEM_HT_JASS_H
     
#define YDTRIGGER_SET_PROC(type) YDNL \
  function YDTRIGGER_SET_##type takes hashtable ht, integer table, integer key, type h returns boolean YDNL \
    if h == null then YDNL \
      call YDTRIGGER_HC_##type##(CLEAR)(ht, table, key) YDNL \
      return true YDNL \
    endif YDNL \
    return YDTRIGGER_##type##(SET)(ht, table, key, h)                                               YDNL \
  endfunction
     
#define YDTRIGGER_SET_integer(ht, table, key, h)   YDTRIGGER_integer(SET)(ht, table, key, h)
#define YDTRIGGER_SET_real(ht, table, key, h)      YDTRIGGER_real(SET)(ht, table, key, h)
#define YDTRIGGER_SET_string(ht, table, key, h)    YDTRIGGER_string(SET)(ht, table, key, h)
#define YDTRIGGER_SET_boolean(ht, table, key, h)   YDTRIGGER_boolean(SET)(ht, table, key, h)
#define YDTRIGGER_SET_StringExt(ht, table, key, h) YDTRIGGER_string(SET)(ht, table, key, h)
#define YDTRIGGER_SET_unitcode(ht, table, key, h)  YDTRIGGER_integer(SET)(ht, table, key, h)
#define YDTRIGGER_SET_abilcode(ht, table, key, h)  YDTRIGGER_integer(SET)(ht, table, key, h)
#define YDTRIGGER_SET_itemcode(ht, table, key, h)  YDTRIGGER_integer(SET)(ht, table, key, h)
#define YDTRIGGER_SET_radian(ht, table, key, h)    YDTRIGGER_real(SET)(ht, table, key, h)
#define YDTRIGGER_SET_degree(ht, table, key, h)    YDTRIGGER_real(SET)(ht, table, key, h)
#define YDTRIGGER_SET_imagefile(ht, table, key, h) YDTRIGGER_string(SET)(ht, table, key, h)
#define YDTRIGGER_SET_modelfile(ht, table, key, h) YDTRIGGER_string(SET)(ht, table, key, h)

YDTRIGGER_SET_PROC(timer)
YDTRIGGER_SET_PROC(trigger)
YDTRIGGER_SET_PROC(unit) 
YDTRIGGER_SET_PROC(item) 
YDTRIGGER_SET_PROC(group)
YDTRIGGER_SET_PROC(player) 
YDTRIGGER_SET_PROC(location)  
YDTRIGGER_SET_PROC(destructable)
YDTRIGGER_SET_PROC(force)
YDTRIGGER_SET_PROC(rect)
YDTRIGGER_SET_PROC(region)
YDTRIGGER_SET_PROC(sound)
YDTRIGGER_SET_PROC(effect) 
YDTRIGGER_SET_PROC(unitpool)
YDTRIGGER_SET_PROC(itempool)
YDTRIGGER_SET_PROC(quest)
YDTRIGGER_SET_PROC(questitem)
YDTRIGGER_SET_PROC(timerdialog)
YDTRIGGER_SET_PROC(leaderboard)
YDTRIGGER_SET_PROC(multiboard)
YDTRIGGER_SET_PROC(multiboarditem)
YDTRIGGER_SET_PROC(trackable)
YDTRIGGER_SET_PROC(dialog)
YDTRIGGER_SET_PROC(button)  
YDTRIGGER_SET_PROC(texttag)
YDTRIGGER_SET_PROC(lightning)
YDTRIGGER_SET_PROC(image)
YDTRIGGER_SET_PROC(fogstate)
YDTRIGGER_SET_PROC(fogmodifier)
 
#undef YDTRIGGER_SET_PROC

#endif
