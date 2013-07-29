#pragma once

#if defined(YDWE_BASE_INLINE)
#	define YDWE_BASE_API
#else
#	if defined(YDWE_BASE_EXPORTS)
#		define YDWE_BASE_API __declspec(dllexport)
#	else
#		define YDWE_BASE_API __declspec(dllimport)
#	endif
#endif
