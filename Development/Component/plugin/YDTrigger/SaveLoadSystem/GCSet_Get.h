# /*
#  *  YDTrigger专用存储系统
#  *  
#  *  set 和 get 的实现
#  *
#  *  By actboy168
#  *
#  */
#
#
#ifndef INCLUDE_YDTRIGGER_SET_AND_GET_H
#define INCLUDE_YDTRIGGER_SET_AND_GET_H
#
# /*
#  *  
#  *  GameCache版 Common
#  *
#  */
#
#  define YDTRIGGER_GAMECACHE                           YDGC
#
# /*
#  *  
#  *  GameCache版 Clear
#  *
#  */
#
#  define YDTriggerClearAll()                           FlushGameCache(YDTRIGGER_GAMECACHE)
#  define YDTriggerClearTable(table)                    FlushStoredMission(YDTRIGGER_GAMECACHE, I2S(table))
#  define YDTriggerClearTableByString(table)            FlushStoredMission(YDTRIGGER_GAMECACHE, table)
#
# /*
#  *  
#  *  GameCache版 Set
#  *
#  */
#
#  define YDTriggerSet(type, table, key, value)         YDTRIGGER_SET_##type##(I2S(table), I2S(key), value)
#  define YDTriggerSetEx(type, table, key, value)       YDTRIGGER_SET_##type##(I2S(table), #key, value)
#  define YDTriggerSetByString(type, table, key, value) YDTRIGGER_SET_##type##(table, key, value) 
#                                                                         
#  define YDTRIGGER_SET_integer                         YDTRIGGER_GCSET_INTEGER
#  define YDTRIGGER_SET_real                            YDTRIGGER_GCSET_REAL
#  define YDTRIGGER_SET_boolean                         YDTRIGGER_GCSET_BOOLEAN
#  define YDTRIGGER_SET_string                          YDTRIGGER_GCSET_STRING
#  define YDTRIGGER_SET_StringExt                       YDTRIGGER_GCSET_STRING
#  define YDTRIGGER_SET_timer                           YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_trigger                         YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_unit                            YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_unitcode                        YDTRIGGER_GCSET_INTEGER
#  define YDTRIGGER_SET_abilcode                        YDTRIGGER_GCSET_INTEGER
#  define YDTRIGGER_SET_item                            YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_itemcode                        YDTRIGGER_GCSET_INTEGER
#  define YDTRIGGER_SET_group                           YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_player                          YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_location                        YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_destructable                    YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_force                           YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_rect                            YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_region                          YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_sound                           YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_effect                          YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_unitpool                        YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_itempool                        YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_quest                           YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_questitem                       YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_timerdialog                     YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_leaderboard                     YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_multiboard                      YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_multiboarditem                  YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_trackable                       YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_dialog                          YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_button                          YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_texttag                         YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_lightning                       YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_image                           YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_fogstate                        YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_fogmodifier                     YDTRIGGER_GCSET_HANDLE
#  define YDTRIGGER_SET_radian                          YDTRIGGER_GCSET_REAL
#  define YDTRIGGER_SET_degree                          YDTRIGGER_GCSET_REAL
#  define YDTRIGGER_SET_imagefile                       YDTRIGGER_GCSET_STRING
#  define YDTRIGGER_SET_modelfile                       YDTRIGGER_GCSET_STRING
#
#  define YDTRIGGER_GCSET_HANDLE(table, key, value)     StoreInteger(YDTRIGGER_GAMECACHE, table, key, YDTRIGGER_H2I(value))
#  define YDTRIGGER_GCSET_INTEGER(table, key, value)    StoreInteger(YDTRIGGER_GAMECACHE, table, key, value)
#  define YDTRIGGER_GCSET_REAL(table, key, value)       StoreReal(YDTRIGGER_GAMECACHE, table, key, value)
#  define YDTRIGGER_GCSET_BOOLEAN(table, key, value)    StoreBoolean(YDTRIGGER_GAMECACHE, table, key, value)
#  define YDTRIGGER_GCSET_STRING(table, key, value)     StoreString(YDTRIGGER_GAMECACHE, table, key, value)
#
# /*
#  *  
#  *  GameCache版 Get
#  *
#  */
#
#  define YDTriggerGet(type, table, key)                YDTRIGGER_GET_##type##(I2S(table), I2S(key)) 
#  define YDTriggerGetEx(type, table, key)              YDTRIGGER_GET_##type##(I2S(table), #key) 
#  define YDTriggerGetByString(type, table, key)        YDTRIGGER_GET_##type##(table, key) 
#
#  define YDTRIGGER_GET_integer                         YDTRIGGER_GCGET_INTEGER
#  define YDTRIGGER_GET_real                            YDTRIGGER_GCGET_REAL
#  define YDTRIGGER_GET_boolean                         YDTRIGGER_GCGET_BOOLEAN
#  define YDTRIGGER_GET_string                          YDTRIGGER_GCGET_STRING
#  define YDTRIGGER_GET_StringExt                       YDTRIGGER_GCGET_STRING
#  define YDTRIGGER_GET_unitcode                        YDTRIGGER_GCGET_INTEGER
#  define YDTRIGGER_GET_abilcode                        YDTRIGGER_GCGET_INTEGER
#  define YDTRIGGER_GET_itemcode                        YDTRIGGER_GCGET_INTEGER
#  define YDTRIGGER_GET_radian                          YDTRIGGER_GCGET_REAL
#  define YDTRIGGER_GET_degree                          YDTRIGGER_GCGET_REAL
#  define YDTRIGGER_GET_imagefile                       YDTRIGGER_GCGET_STRING
#  define YDTRIGGER_GET_modelfile                       YDTRIGGER_GCGET_STRING
#
#  define YDTRIGGER_GCGET_INTEGER(table, key)           GetStoredInteger(YDTRIGGER_GAMECACHE, table, key)
#  define YDTRIGGER_GCGET_REAL(table, key)              GetStoredReal(YDTRIGGER_GAMECACHE, table, key)
#  define YDTRIGGER_GCGET_BOOLEAN(table, key)           GetStoredBoolean(YDTRIGGER_GAMECACHE, table, key)
#  define YDTRIGGER_GCGET_STRING(table, key)            GetStoredString(YDTRIGGER_GAMECACHE, table, key)
#
#endif
