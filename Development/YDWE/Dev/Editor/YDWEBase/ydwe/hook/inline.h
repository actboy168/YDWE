#pragma once

#include <ydwe/config.h>
#include <cstdint>

namespace ydwe { namespace hook {
	YDWE_BASE_API bool inline_install(uintptr_t* pointer_ptr, uintptr_t detour);
	YDWE_BASE_API bool inline_uninstall(uintptr_t* pointer_ptr, uintptr_t detour);	
	YDWE_BASE_API uintptr_t replace_pointer(uintptr_t address, uintptr_t new_value);
}}
