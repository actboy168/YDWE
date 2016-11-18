#pragma once

#if defined(YDWE_BASE_INLINE)
#	define _BASE_API
#else
#	if defined(YDWE_BASE_EXPORTS)
#		define _BASE_API __declspec(dllexport)
#	else
#		define _BASE_API __declspec(dllimport)
#	endif
#endif

#if _MSC_VER >= 1910
#else
#define noexcept
#endif
