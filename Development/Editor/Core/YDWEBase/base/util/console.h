#pragma once

#include <base/config.h>

namespace base { namespace util {
	class _BASE_API console
	{
	public:
		static void enable();
		static bool disable_close_button();
		static void pause();
	};
}}
