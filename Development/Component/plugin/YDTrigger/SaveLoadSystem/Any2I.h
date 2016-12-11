# /*
#  *  YDTrigger专用存储系统
#  *  
#  *  Any2I
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_ANY2I_H
#define INCLUDE_YDTRIGGER_ANY2I_H
#
#  define YDTriggerAny2I(type, any)                            YDTRIGGER_ANY2I_##type##(any)
#
#  define YDTriggerI2I(any)                                    any
#  define YDTriggerR2I(any)                                    R2I(any)
#  define YDTriggerB2I(any)                                    ERROR "<错误>不能从布尔值转换成整数！"        // 故意的错误
#
#  if WARCRAFT_VERSION >= 124
#    define YDTriggerS2I(any)                                  StringHash(any)
#    define YDTriggerH2I(any)                                  GetHandleId(any)
#  else
#    define YDTriggerS2I(any)                                  YDTRIGGER_S2I(any)
#    define YDTriggerH2I(any)                                  YDTRIGGER_H2I(any)
#  endif
#                                          
#  define YDTRIGGER_ANY2I_integer(any)                         YDTriggerI2I(any)
#  define YDTRIGGER_ANY2I_real(any)                            YDTriggerR2I(any)
#  define YDTRIGGER_ANY2I_boolean(any)                         YDTriggerB2I(any)
#  define YDTRIGGER_ANY2I_string(any)                          YDTriggerS2I(any)
#  define YDTRIGGER_ANY2I_timer(any)                           YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_trigger(any)                         YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_unit(any)                            YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_unitcode(any)                        YDTriggerI2I(any)
#  define YDTRIGGER_ANY2I_abilcode(any)                        YDTriggerI2I(any)
#  define YDTRIGGER_ANY2I_item(any)                            YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_itemcode(any)                        YDTriggerI2I(any)
#  define YDTRIGGER_ANY2I_group(any)                           YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_player(any)                          YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_location(any)                        YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_destructable(any)                    YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_force(any)                           YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_rect(any)                            YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_region(any)                          YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_sound(any)                           YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_effect(any)                          YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_unitpool(any)                        YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_itempool(any)                        YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_quest(any)                           YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_questitem(any)                       YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_timerdialog(any)                     YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_leaderboard(any)                     YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_multiboard(any)                      YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_multiboarditem(any)                  YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_trackable(any)                       YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_dialog(any)                          YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_button(any)                          YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_texttag(any)                         YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_lightning(any)                       YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_image(any)                           YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_fogstate(any)                        YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_fogmodifier(any)                     YDTriggerH2I(any)
#  define YDTRIGGER_ANY2I_radian(any)                          YDTRIGGERR2I(any)
#  define YDTRIGGER_ANY2I_degree(any)                          YDTRIGGERR2I(any)
#  define YDTRIGGER_ANY2I_imagefile(any)                       YDTRIGGERS2I(any)
#  define YDTRIGGER_ANY2I_modelfile(any)                       YDTRIGGERS2I(any)
#
#
#endif
