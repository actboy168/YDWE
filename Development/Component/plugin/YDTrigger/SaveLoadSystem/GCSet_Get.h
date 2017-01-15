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
#  *  GameCache版 Clear
#  *
#  */
#
#  define YDHashClearAll(handle)                             FlushGameCache(handle)
#  define YDHashClearTable(handle, table)                    FlushStoredMission(handle, I2S(table))
#  define YDHashClearTableByString(handle, table)            FlushStoredMission(handle, table)
#
# /*
#  *  
#  *  GameCache版 Set
#  *
#  */
#
#  define YDHashSetEx(handle, type, table, key, value)       YDTRIGGER_SET_##type##(handle, I2S(table), I2S(key), value)
#  define YDHashSet(handle, type, table, key, value)         YDTRIGGER_SET_##type##(handle, I2S(table), #key, value)
#  define YDHashSetByString(handle, type, table, key, value) YDTRIGGER_SET_##type##(handle, table, key, value) 
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
#  define YDTRIGGER_GCSET_HANDLE(handle, table, key, value)  StoreInteger(handle, table, key, YDTRIGGER_H2I(value))
#  define YDTRIGGER_GCSET_INTEGER(handle, table, key, value) StoreInteger(handle, table, key, value)
#  define YDTRIGGER_GCSET_REAL(handle, table, key, value)    StoreReal(handle, table, key, value)
#  define YDTRIGGER_GCSET_BOOLEAN(handle, table, key, value) StoreBoolean(handle, table, key, value)
#  define YDTRIGGER_GCSET_STRING(handle, table, key, value)  StoreString(handle, table, key, value)
#
# /*
#  *  
#  *  GameCache版 Get
#  *
#  */
#
#  define YDTriggerGet(handle, type, table, key)         YDTRIGGER_GET_##type##(handle, I2S(table), I2S(key)) 
#  define YDTriggerGetEx(handle, type, table, key)       YDTRIGGER_GET_##type##(handle, I2S(table), #key) 
#  define YDTriggerGetByString(handle, type, table, key) YDTRIGGER_GET_##type##(handle, table, key) 
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
#  define YDTRIGGER_GCGET_INTEGER(handle, table, key)   GetStoredInteger(handle, table, key)
#  define YDTRIGGER_GCGET_REAL(handle, table, key)      GetStoredReal(handle, table, key)
#  define YDTRIGGER_GCGET_BOOLEAN(handle, table, key)   GetStoredBoolean(handle, table, key)
#  define YDTRIGGER_GCGET_STRING(handle, table, key)    GetStoredString(handle, table, key)
#
#endif
