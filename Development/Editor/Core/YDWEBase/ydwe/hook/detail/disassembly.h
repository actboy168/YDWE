#pragma once

#include <cstdint>

namespace ydwe { namespace hook { namespace detail {
	size_t next_opcode(uintptr_t code, uint8_t** opcode_ptr);
}}}
