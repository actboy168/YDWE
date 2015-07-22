#pragma once

#include <base/config.h>
#include <stdint.h>

namespace base { namespace warcraft3 { namespace message_dispatch {
	_BASE_API void add_filter(uintptr_t filter);
	_BASE_API void remove_filter(uintptr_t filter);
}}}
