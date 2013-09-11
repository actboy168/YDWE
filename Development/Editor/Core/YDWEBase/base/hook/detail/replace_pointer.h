#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { 
namespace hook { namespace detail {
	uintptr_t replace_pointer(uintptr_t address, uintptr_t new_value);
}}
}
