#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { 
namespace hook { namespace detail {
	_BASE_API size_t next_opcode(uintptr_t code, uint8_t** opcode_ptr);
}}
}
