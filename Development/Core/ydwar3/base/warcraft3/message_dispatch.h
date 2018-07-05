#pragma once

#include <base/warcraft3/config.h>
#include <stdint.h>

namespace base { namespace warcraft3 { namespace message_dispatch {
	_WAR3_API void add_filter(uintptr_t filter);
	_WAR3_API void remove_filter(uintptr_t filter);
}}}
