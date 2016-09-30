#pragma once

#include <base/config.h>
#include <Windows.h>

namespace base { 
namespace hook { namespace detail {
	bool inject_dll(HANDLE process_handle, HANDLE thread_handle, const wchar_t* dll_name);
}}
}
