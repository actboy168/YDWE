#pragma once

#include <ydwe/config.h>
#include <cstdint>

_BASE_BEGIN 
namespace hook { namespace detail {
	size_t next_opcode(uintptr_t code, uint8_t** opcode_ptr);
}}
_BASE_END
