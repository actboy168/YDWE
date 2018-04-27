#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { namespace hook {
	uintptr_t _BASE_API replace_pointer(uintptr_t address, uintptr_t new_value);
}}
