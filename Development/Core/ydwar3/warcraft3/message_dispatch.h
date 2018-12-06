#pragma once

#include <warcraft3/config.h>
#include <stdint.h>

namespace warcraft3::message_dispatch {
	_WAR3_API void add_filter(uintptr_t filter);
	_WAR3_API void remove_filter(uintptr_t filter);
}
