#pragma once

#include <ydwe/config.h>
#include <Windows.h>

namespace ydwe { namespace hook {	
	YDWE_BASE_API uintptr_t eat(HMODULE module_handle,      const char* api_name, uintptr_t new_function);
	YDWE_BASE_API uintptr_t eat(const wchar_t* module_name, const char* api_name, uintptr_t new_function);
}}
