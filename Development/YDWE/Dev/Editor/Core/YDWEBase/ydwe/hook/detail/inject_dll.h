#pragma once

#include <Windows.h>

namespace ydwe { namespace hook { namespace detail {
	bool inject_dll(HANDLE process_handle, HANDLE thread_handle, const wchar_t* dll_name);
}}}
