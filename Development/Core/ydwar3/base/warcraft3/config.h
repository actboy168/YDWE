#pragma once

#if defined(YDWE_WAR3_INLINE)
#	define _WAR3_API
#else
#	if defined(YDWE_WAR3_EXPORTS)
#		define _WAR3_API __declspec(dllexport)
#	else
#		define _WAR3_API __declspec(dllimport)
#	endif
#endif

#if _MSC_VER >= 1910
#else
#define noexcept
#endif
