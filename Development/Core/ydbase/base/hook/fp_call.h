#pragma once

#include <type_traits>

#if !defined(_MSC_VER) || _MSC_VER >= 1800

#include <base/util/horrible_cast.h>

namespace base {

	namespace call_ {
		template <class OutputClass, class InputClass>
		struct same_size {
			static const bool value = 
				(!std::is_reference<InputClass>::value && sizeof(OutputClass) == sizeof(InputClass))
				|| (std::is_reference<InputClass>::value && sizeof(OutputClass) == sizeof(std::add_pointer<InputClass>::type));
		};

		template <class OutputClass, class InputClass>
		union cast_union
		{
			OutputClass out;
			InputClass in;
		};

		template <class Argument>
		inline uintptr_t cast(const Argument input, typename std::enable_if<std::is_function<typename std::remove_reference<Argument>::type>::value, void>::type* = 0)
		{
			cast_union<uintptr_t, Argument> u;
			static_assert(std::is_pod<Argument>::value, "Argument is not a pod.");
			static_assert((sizeof(Argument) == sizeof(u)) && (sizeof(Argument) == sizeof(uintptr_t)), "Argument and uintptr_t are not the same size.");
			u.in = input;
			return u.out;
		}

		template <class Argument>
		inline uintptr_t cast(const Argument input, typename std::enable_if<!std::is_function<typename std::remove_reference<Argument>::type>::value && same_size<uintptr_t, Argument>::value, void>::type* = 0)
		{
			cast_union<uintptr_t, Argument> u;
			static_assert(std::is_pod<Argument>::value, "Argument is not a pod.");
			u.in = input;
			return u.out;
		}

		template <class Argument>
		inline uintptr_t cast(const Argument input, typename std::enable_if<!std::is_function<typename std::remove_reference<Argument>::type>::value && !same_size<uintptr_t, Argument>::value, void>::type* = 0)
		{
			static_assert(std::is_pod<Argument>::value, "Argument is not a pod.");
			static_assert(sizeof(Argument) < sizeof(uintptr_t), "Argument can not be converted to uintptr_t.");
			return static_cast<uintptr_t>(input);
		}

		template <typename Arg>
		struct cast_type {
			typedef uintptr_t type;
		};
	}

	template <typename R, typename F, typename ... Args>
	inline R std_call(F f, Args ... args)
	{
		return (reinterpret_cast<R(__stdcall *)(typename call_::cast_type<Args>::type ... args)>(f))(call_::cast(args)...);
	}

	template <typename R, typename F, typename ... Args>
	inline R fast_call(F f, Args ... args)
	{
		return (reinterpret_cast<R(__fastcall *)(typename call_::cast_type<Args>::type ... args)>(f))(call_::cast(args)...);
	}

	template <typename R, typename F, typename ... Args>
	inline R c_call(F f, Args ... args)
	{
		return (reinterpret_cast<R(__cdecl *)(typename call_::cast_type<Args>::type ... args)>(f))(call_::cast(args)...);
	}

	template <typename R, typename F, typename This, typename ... Args>
	inline R this_call(F f, This t, Args ... args)
	{
		return (reinterpret_cast<R(__fastcall *)(typename call_::cast_type<This>::type, void*, typename call_::cast_type<Args>::type ... args)>(f))(call_::cast(t), 0, call_::cast(args)...);
	}
}

#else

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/stringize.hpp>

namespace base {

#define DEFINE_FP_CALL_ASSERT(z, n, param) \
	static_assert(sizeof(uintptr_t) >= sizeof(BOOST_PP_CAT(param, n)), "sizeof(uintptr_t) != sizeof(" ## BOOST_PP_STRINGIZE(BOOST_PP_CAT(param, n)) ## ")"); \
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

#endif
