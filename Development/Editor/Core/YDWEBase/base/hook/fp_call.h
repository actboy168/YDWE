#pragma once

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <type_traits>

namespace base {

#define DEFINE_FP_CALL_ASSERT(z, n, param) \
	static_assert(sizeof(uintptr_t) == sizeof(BOOST_PP_CAT(param, n)), "sizeof(uintptr_t) != sizeof(" ## BOOST_PP_STRINGIZE(BOOST_PP_CAT(param, n)) ## ")"); \
	static_assert(!std::is_reference<BOOST_PP_CAT(param, n)>::value, BOOST_PP_STRINGIZE(BOOST_PP_CAT(param, n)) ## "is a reference)"); \

#define DEFINE_FP_CALL(z, n, unused) \
	template <typename ReturnType, typename FPType BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename A)> \
	inline ReturnType std_call(FPType fp BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)) \
	{ \
		BOOST_PP_REPEAT(n, DEFINE_FP_CALL_ASSERT, A) \
		typedef ReturnType(__stdcall *TFPAeroFunction)(BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)); \
		return ((TFPAeroFunction)fp)(BOOST_PP_ENUM_PARAMS(n, a)); \
	} \
	template <typename ReturnType, typename FPType BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename A)> \
	inline ReturnType fast_call(FPType fp BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)) \
	{ \
		BOOST_PP_REPEAT(n, DEFINE_FP_CALL_ASSERT, A) \
		typedef ReturnType(__fastcall *TFPAeroFunction)(BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)); \
		return ((TFPAeroFunction)fp)(BOOST_PP_ENUM_PARAMS(n, a)); \
	} \
	template <typename ReturnType, typename FPType BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename A)> \
	inline ReturnType c_call(FPType fp BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)) \
	{ \
		BOOST_PP_REPEAT(n, DEFINE_FP_CALL_ASSERT, A) \
		typedef ReturnType(__cdecl *TFPAeroFunction)(BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)); \
		return ((TFPAeroFunction)fp)(BOOST_PP_ENUM_PARAMS(n, a)); \
	} \
	template <typename ReturnType, typename FPType, typename T0 BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename A)> \
	inline ReturnType this_call(FPType fp, T0 t0 BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)) \
	{ \
		BOOST_PP_REPEAT(n, DEFINE_FP_CALL_ASSERT, A) \
		DEFINE_FP_CALL_ASSERT(z, 0, T) \
		typedef ReturnType(__fastcall *TFPAeroFunction)(T0 t0, void * BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, A, a)); \
		return ((TFPAeroFunction)fp)(t0, 0 BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, a)); \
	}

BOOST_PP_REPEAT(16, DEFINE_FP_CALL, ~)

#undef DEFINE_FP_CALL
#undef DEFINE_FP_CALL_ASSERT

}
