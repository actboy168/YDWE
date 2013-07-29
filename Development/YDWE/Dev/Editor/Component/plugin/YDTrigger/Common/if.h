# /*
#  *  ifºê
#  *  
#  *  YDTRIGGER_COMMON_IF(true,  t, f) -> t
#  *  YDTRIGGER_COMMON_IF(false, t, f) -> f
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_COMMON_IF_H
#define INCLUDE_YDTRIGGER_COMMON_IF_H
#
#  define YDTRIGGER_COMMON_IF(cond, tt, ff)   YDTRIGGER_COMMON_IF_I(cond, tt, ff)
#  define YDTRIGGER_COMMON_IF_I(cond, tt, ff) YDTRIGGER_COMMON_IF_CHECK_ ## cond(tt, ff)
#
#  define YDTRIGGER_COMMON_IF_CHECK_true(tt, ff)        tt
#  define YDTRIGGER_COMMON_IF_CHECK_false(tt, ff)       ff
#
#endif
