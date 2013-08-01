#pragma once

#include <ydwe/config.h>
#include <Windows.h>

_BASE_BEGIN 
namespace hook { namespace detail {
	bool inject_dll(HANDLE process_handle, HANDLE thread_handle, const wchar_t* dll_name);
}}
_BASE_END
