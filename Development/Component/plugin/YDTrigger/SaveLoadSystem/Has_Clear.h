# /*
#  *  YDTrigger专用存储系统
#  *  
#  *  has 和 clear 的实现
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_HAS_AND_CLEAR_H
#define INCLUDE_YDTRIGGER_HAS_AND_CLEAR_H
#
#    if WARCRAFT_VERSION >= 124
#      define YDHashClear(handle, type, table, key)         YDTRIGGER_HC_##type##(CLEAR)(handle, table, key)
#      define YDHashHas(handle, type, table, key)           YDTRIGGER_HC_##type##(HAS)(handle, table, key)
#
#      define YDTRIGGER_CLEAR(Type)                         RemoveSaved##Type
#      define YDTRIGGER_HAS(Type)                           HaveSaved##Type
#
#      define RemoveSavedHandleDummy(ht, table, key)        RemoveSavedHandle(ht, table, key)
#      define HaveSavedHandleDummy(ht, table, key)          HaveSavedHandle(ht, table, key)
#    else
#      define YDHashClear(handle, type, table, key)         YDTRIGGER_HC_##type##(CLEAR)(handle, I2S(table), #key)
#      define YDHashHas(handle, type, table, key)           YDTRIGGER_HC_##type##(HAS)(handle, I2S(table), #key)
#
#      define YDTRIGGER_CLEAR(Type)                         FlushStored##Type
#      define YDTRIGGER_HAS(Type)                           HaveStored##Type
#
#      define FlushStoredHandleDummy(gc, table, key)        FlushStoredInteger(gc, table, key)
#      define HaveStoredHandleDummy(gc, table, key)         HaveStoredInteger(gc, table, key)
#    endif
#
#  define YDTRIGGER_HC_integer(HC)                          YDTRIGGER_##HC##(Integer)
#  define YDTRIGGER_HC_real(HC)                             YDTRIGGER_##HC##(Real)
#  define YDTRIGGER_HC_boolean(HC)                          YDTRIGGER_##HC##(Boolean)
#  define YDTRIGGER_HC_string(HC)                           YDTRIGGER_##HC##(String)
#  define YDTRIGGER_HC_StringExt(HC)                        YDTRIGGER_##HC##(String)
#  define YDTRIGGER_HC_timer(HC)                            YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_trigger(HC)                          YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_unit(HC)                             YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_unitcode(HC)                         YDTRIGGER_##HC##(Integer)
#  define YDTRIGGER_HC_abilcode(HC)                         YDTRIGGER_##HC##(Integer)
#  define YDTRIGGER_HC_item(HC)                             YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_itemcode(HC)                         YDTRIGGER_##HC##(Integer)
#  define YDTRIGGER_HC_group(HC)                            YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_player(HC)                           YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_location(HC)                         YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_destructable(HC)                     YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_force(HC)                            YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_rect(HC)                             YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_region(HC)                           YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_sound(HC)                            YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_effect(HC)                           YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_unitpool(HC)                         YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_itempool(HC)                         YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_quest(HC)                            YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_questitem(HC)                        YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_timerdialog(HC)                      YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_leaderboard(HC)                      YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_multiboard(HC)                       YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_multiboarditem(HC)                   YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_trackable(HC)                        YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_dialog(HC)                           YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_button(HC)                           YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_texttag(HC)                          YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_lightning(HC)                        YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_image(HC)                            YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_fogstate(HC)                         YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_fogmodifier(HC)                      YDTRIGGER_##HC##(HandleDummy)
#  define YDTRIGGER_HC_radian(HC)                           YDTRIGGER_##HC##(Real)
#  define YDTRIGGER_HC_degree(HC)                           YDTRIGGER_##HC##(Real)
#  define YDTRIGGER_HC_imagefile(HC)                        YDTRIGGER_##HC##(String)
#  define YDTRIGGER_HC_modelfile(HC)                        YDTRIGGER_##HC##(String)
#
#
#endif
