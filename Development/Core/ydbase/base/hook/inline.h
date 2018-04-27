#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { namespace hook {
	typedef void* hook_t;
	_BASE_API bool install(uintptr_t* pointer_ptr, uintptr_t detour, hook_t* ph = nullptr);
	_BASE_API bool uninstall(hook_t* ph);
}}
