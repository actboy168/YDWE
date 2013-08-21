#pragma once

#include <ydwe/config.h>

namespace ydwe { namespace util {
	class YDWE_BASE_API console
	{
	public:
		static void enable();
		static bool disable_close_button();
		static void pause();
	};
}}
