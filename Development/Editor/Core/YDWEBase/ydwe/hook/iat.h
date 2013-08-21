#pragma once

#include <ydwe/config.h>
#include <Windows.h>
#include <list>

namespace ydwe { namespace hook {	
	uintptr_t YDWE_BASE_API iat(HMODULE module_handle,      const char* dll_name, const char* api_name, uintptr_t new_function);
	uintptr_t YDWE_BASE_API iat(const wchar_t* module_name, const char* dll_name, const char* api_name, uintptr_t new_function);
}}
