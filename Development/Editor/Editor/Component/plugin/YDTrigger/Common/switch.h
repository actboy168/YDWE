# /*
#  *  switchºê
#  *  
#  *  YDTRIGGER_COMMON_SWITCH(1, (A, B, C)) -> B
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_COMMON_SWITCH_H
#define INCLUDE_YDTRIGGER_COMMON_SWITCH_H
#
#  define YDTRIGGER_COMMON_SWITCH(cond, list)   YDTRIGGER_COMMON_SWITCH_I(cond, list)
#  define YDTRIGGER_COMMON_SWITCH_I(cond, list) YDTRIGGER_COMMON_SWITCH_CHECK_ ## cond list

#  define YDTRIGGER_COMMON_SWITCH_CHECK_0(e0, ...)                      e0
#  define YDTRIGGER_COMMON_SWITCH_CHECK_1(e0,e1, ...)                   e1
#  define YDTRIGGER_COMMON_SWITCH_CHECK_2(e0,e1,e2, ...)                e2
#  define YDTRIGGER_COMMON_SWITCH_CHECK_3(e0,e1,e2,e3, ...)             e3
#  define YDTRIGGER_COMMON_SWITCH_CHECK_4(e0,e1,e2,e3,e4, ...)          e4
#  define YDTRIGGER_COMMON_SWITCH_CHECK_5(e0,e1,e2,e3,e4,e5, ...)       e5
#  define YDTRIGGER_COMMON_SWITCH_CHECK_6(e0,e1,e2,e3,e4,e5,e6, ...)    e6
#  define YDTRIGGER_COMMON_SWITCH_CHECK_7(e0,e1,e2,e3,e4,e5,e6,e7, ...) e7
#                                                                      
#endif
