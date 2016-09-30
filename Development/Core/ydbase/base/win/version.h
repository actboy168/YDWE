#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { namespace win {
	enum version 
	{
		VERSION_PRE_XP = 0,
		VERSION_XP,
		VERSION_SERVER_2003,
		VERSION_VISTA,
		VERSION_WIN7,
		VERSION_WIN8,
		VERSION_WIN8_1,
		VERSION_WIN10,
		VERSION_WIN_LAST,
	};

	struct version_number
	{
		uint32_t major;
		uint32_t minor;
		uint32_t build;
	};

	_BASE_API version_number get_version_number();
	_BASE_API version        get_version();
}}
