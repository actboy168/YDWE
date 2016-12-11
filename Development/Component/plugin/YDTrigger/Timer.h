#include <YDTrigger/ImportSaveLoadSystem.h>
#include <boost/preprocessor/slot/counter.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
  
  
#include BOOST_PP_UPDATE_COUNTER()

#ifdef YDTimerPragmaSize
#undef YDTimerPragmaSize
#endif
#ifdef YDTimerPragmaTuple
#undef YDTimerPragmaTuple
#endif
#ifdef YDTimerBindHash
#undef YDTimerBindHash
#endif
#ifdef YDTimerReadHash
#undef YDTimerReadHash
#endif
#ifdef YDTimerBright
#undef YDTimerBright
#endif
#ifdef YDTimerStartEx
#undef YDTimerStartEx
#endif

#define YDTimerPragmaSize   BOOST_PP_TUPLE_ELEM(3, 1, YDTimerBind)
#define YDTimerPragmaTuple  BOOST_PP_TUPLE_ELEM(3, 2, YDTimerBind)
#define YDTimerBindHash(t, type, var) YDTriggerSet(type, YDTriggerAny2I(timer, t), StringHash(#var), var)
#define YDTimerReadHash(t, type, var) var = YDTriggerGet(type, YDTriggerAny2I(timer, t), StringHash(#var))
#define YDTimerBright   BOOST_PP_CAT(YDTimerBright_, BOOST_PP_COUNTER)
#define YDTimerStartEx  BOOST_PP_CAT(YDTimerStartEx_,  BOOST_PP_COUNTER)

#ifndef YDTimerPragmaListHasType
#define YDTimerPragmaListHasType(z, n, data) BOOST_PP_TUPLE_ELEM(YDTimerPragmaSize, n, data) p ## n
#endif

#ifndef YDTimerPragmaList
#define YDTimerPragmaList(z, n, data) p ## n
#endif

function YDTimerBright takes nothing returns nothing
	local timer t = GetExpiredTimer()
#define BOOST_PP_LOCAL_MACRO(n) local BOOST_PP_TUPLE_ELEM(YDTimerPragmaSize, n, YDTimerPragmaTuple) YDTimerReadHash(t, BOOST_PP_TUPLE_ELEM(YDTimerPragmaSize, n, YDTimerPragmaTuple), p ## n)
#define BOOST_PP_LOCAL_LIMITS   (0, BOOST_PP_SUB(YDTimerPragmaSize,1))
#include BOOST_PP_LOCAL_ITERATE()
    call BOOST_PP_TUPLE_ELEM(3, 0, YDTimerBind)(BOOST_PP_ENUM(YDTimerPragmaSize, YDTimerPragmaList, YDTimerPragmaTuple))
endfunction

function YDTimerStartEx takes timer t, real timeout, boolean periodic, BOOST_PP_ENUM(YDTimerPragmaSize, YDTimerPragmaListHasType, YDTimerPragmaTuple) returns nothing
#define BOOST_PP_LOCAL_MACRO(n) call YDTimerBindHash(t, BOOST_PP_TUPLE_ELEM(YDTimerPragmaSize, n, YDTimerPragmaTuple), p ## n)
#define BOOST_PP_LOCAL_LIMITS   (0, BOOST_PP_SUB(YDTimerPragmaSize,1))
#include BOOST_PP_LOCAL_ITERATE()
    call TimerStart(t, timeout, periodic, function YDTimerBright)
endfunction

#ifdef YDTimerBind
#undef YDTimerBind
#endif
