#pragma once

#include <cstdint>

namespace ydwe { namespace hook { namespace detail {
	uintptr_t replace_pointer(uintptr_t address, uintptr_t new_value);
}}}
