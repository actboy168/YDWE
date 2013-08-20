#pragma once

#include <ydwe/config.h>
#include <cstdint>

_BASE_BEGIN 
namespace hook { namespace detail {
	uintptr_t replace_pointer(uintptr_t address, uintptr_t new_value);
}}
_BASE_END
