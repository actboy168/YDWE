# /*
#  *  loopºê
#  *  
#  *  #define  YDTRIGGER_COMMON_LOOP(n) set i = n
#  *  #define  YDTRIGGER_COMMON_LOOP_LIMITS (0, 2)
#  *  #include <YDTrigger/Common/loop.h>
#  *    ->
#  *    set i = 0
#  *    set i = 1
#  *
#  *  By actboy168
#  *
#  */
#
#include <boost/preprocessor/iteration/local.hpp>
#define BOOST_PP_LOCAL_MACRO(n) YDTRIGGER_COMMON_LOOP(n)
#define BOOST_PP_LOCAL_LIMITS   YDTRIGGER_COMMON_LOOP_LIMITS
#include BOOST_PP_LOCAL_ITERATE()
#undef  YDTRIGGER_COMMON_LOOP
#undef  YDTRIGGER_COMMON_LOOP_LIMITS
