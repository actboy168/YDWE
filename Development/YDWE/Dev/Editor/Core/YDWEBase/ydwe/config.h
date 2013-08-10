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

#define _BASE       ydwe
#define _BASE_BEGIN namespace _BASE {
#define _BASE_END   }
