#pragma once

#if defined(_MSC_VER)
#elif defined(__GCC__) || defined(__GNUC__)
#	include <slk/port/port_gcc.h>
#else
#	error unknown compiler
#endif

#if defined(SLKLIB_INLINE)
#	define SLKLIB_API
#else
#	if defined(SLKLIB_EXPORTS)
#		define SLKLIB_API __declspec(dllexport)
#	else
#		define SLKLIB_API __declspec(dllimport)
#	endif
#endif

#pragma warning(disable:4251)
