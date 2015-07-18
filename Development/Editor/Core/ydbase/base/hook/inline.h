#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { namespace hook {  
#if !defined(DISABLE_DETOURS)
	_BASE_API bool inline_install(uintptr_t* pointer_ptr, uintptr_t detour);
	_BASE_API bool inline_uninstall(uintptr_t* pointer_ptr, uintptr_t detour);
#endif
	_BASE_API uintptr_t replace_pointer(uintptr_t address, uintptr_t new_value);
}}
