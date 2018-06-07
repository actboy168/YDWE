# /*
#  *  YDTriggerר�ô洢ϵͳ
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
#  define YDHashAny2I(type, any)                            YDTRIGGER_ANY2I_##type##(any)
#
#  define YDHashI2I(any)                                    any
#  define YDHashR2I(any)                                    R2I(any)
#  define YDHashB2I(any)                                    ERROR "<����>���ܴӲ���ֵת����������"        // �����Ĵ���
#
#  if WARCRAFT_VERSION >= 124
#    define YDHashS2I(any)                                  StringHash(any)
#    define YDHashH2I(any)                                  GetHandleId(any)
#  else
#    define YDHashS2I(any)                                  YDTRIGGER_S2I(any)
#    define YDHashH2I(any)                                  YDTRIGGER_H2I(any)
#  endif
#                                          
#  define YDTRIGGER_ANY2I_integer(any)                      YDHashI2I(any)
#  define YDTRIGGER_ANY2I_real(any)                         YDHashR2I(any)
#  define YDTRIGGER_ANY2I_boolean(any)                      YDHashB2I(any)
#  define YDTRIGGER_ANY2I_string(any)                       YDHashS2I(any)
#  define YDTRIGGER_ANY2I_timer(any)                        YDHashH2I(any)
#  define YDTRIGGER_ANY2I_trigger(any)                      YDHashH2I(any)
#  define YDTRIGGER_ANY2I_unit(any)                         YDHashH2I(any)
#  define YDTRIGGER_ANY2I_unitcode(any)                     YDHashI2I(any)
#  define YDTRIGGER_ANY2I_abilcode(any)                     YDHashI2I(any)
#  define YDTRIGGER_ANY2I_item(any)                         YDHashH2I(any)
#  define YDTRIGGER_ANY2I_itemcode(any)                     YDHashI2I(any)
#  define YDTRIGGER_ANY2I_group(any)                        YDHashH2I(any)
#  define YDTRIGGER_ANY2I_player(any)                       YDHashH2I(any)
#  define YDTRIGGER_ANY2I_location(any)                     YDHashH2I(any)
#  define YDTRIGGER_ANY2I_destructable(any)                 YDHashH2I(any)
#  define YDTRIGGER_ANY2I_force(any)                        YDHashH2I(any)
#  define YDTRIGGER_ANY2I_rect(any)                         YDHashH2I(any)
#  define YDTRIGGER_ANY2I_region(any)                       YDHashH2I(any)
#  define YDTRIGGER_ANY2I_sound(any)                        YDHashH2I(any)
#  define YDTRIGGER_ANY2I_effect(any)                       YDHashH2I(any)
#  define YDTRIGGER_ANY2I_unitpool(any)                     YDHashH2I(any)
#  define YDTRIGGER_ANY2I_itempool(any)                     YDHashH2I(any)
#  define YDTRIGGER_ANY2I_quest(any)                        YDHashH2I(any)
#  define YDTRIGGER_ANY2I_questitem(any)                    YDHashH2I(any)
#  define YDTRIGGER_ANY2I_timerdialog(any)                  YDHashH2I(any)
#  define YDTRIGGER_ANY2I_leaderboard(any)                  YDHashH2I(any)
#  define YDTRIGGER_ANY2I_multiboard(any)                   YDHashH2I(any)
#  define YDTRIGGER_ANY2I_multiboarditem(any)               YDHashH2I(any)
#  define YDTRIGGER_ANY2I_trackable(any)                    YDHashH2I(any)
#  define YDTRIGGER_ANY2I_dialog(any)                       YDHashH2I(any)
#  define YDTRIGGER_ANY2I_button(any)                       YDHashH2I(any)
#  define YDTRIGGER_ANY2I_texttag(any)                      YDHashH2I(any)
#  define YDTRIGGER_ANY2I_lightning(any)                    YDHashH2I(any)
#  define YDTRIGGER_ANY2I_image(any)                        YDHashH2I(any)
#  define YDTRIGGER_ANY2I_fogstate(any)                     YDHashH2I(any)
#  define YDTRIGGER_ANY2I_fogmodifier(any)                  YDHashH2I(any)
#  define YDTRIGGER_ANY2I_radian(any)                       YDTRIGGERR2I(any)
#  define YDTRIGGER_ANY2I_degree(any)                       YDTRIGGERR2I(any)
#  define YDTRIGGER_ANY2I_imagefile(any)                    YDTRIGGERS2I(any)
#  define YDTRIGGER_ANY2I_modelfile(any)                    YDTRIGGERS2I(any)
#
#
#endif
