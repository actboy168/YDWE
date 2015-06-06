#pragma once

#include <base/config.h>
#include <Windows.h>

namespace base { namespace hook {	
	_BASE_API bool dyn_iat(HMODULE module_handle, const wchar_t* dll_name, const char* api_name, uintptr_t* old_function, uintptr_t new_function);
	_BASE_API bool dyn_iat(const wchar_t* module_name, const wchar_t* dll_name, const char* api_name, uintptr_t* old_function, uintptr_t new_function);
}}
