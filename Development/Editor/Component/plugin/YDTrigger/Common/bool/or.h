# /*
#  *  orºê
#  *  
#  *  YDTRIGGER_COMMON_BOOL_OR(true, flase) -> true
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_COMMON_BOOL_OR_H
#define INCLUDE_YDTRIGGER_COMMON_BOOL_OR_H
#
#  define YDTRIGGER_COMMON_BOOL_OR(x, y) YDTRIGGER_COMMON_BOOL_OR_I(x, y)
#  define YDTRIGGER_COMMON_BOOL_OR_I(x, y) YDTRIGGER_COMMON_BOOL_OR_CHECK_## x ## _ ## y
#
#  define YDTRIGGER_COMMON_BOOL_OR_CHECK_true_true   true
#  define YDTRIGGER_COMMON_BOOL_OR_CHECK_true_false  true
#  define YDTRIGGER_COMMON_BOOL_OR_CHECK_false_true  true
#  define YDTRIGGER_COMMON_BOOL_OR_CHECK_false_false false
#
#endif
