#pragma once

#include <base/config.h>

_BASE_BEGIN namespace util {
	class _BASE_API console
	{
	public:
		static void enable();
		static bool disable_close_button();
		static void pause();
	};
}}
