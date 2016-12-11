#pragma once

#include <base/config.h>
#include <Windows.h>			  		
#include <base/filesystem.h>

namespace base { namespace path { namespace detail {
	fs::path quick_launch_path(bool default_user);
	fs::path temp_path();
	fs::path windows_path();
	fs::path system_path();
	_BASE_API fs::path module_path(HMODULE module_handle);
	_BASE_API fs::path module_path(HANDLE process_handle, HMODULE module_handle);
}}}
