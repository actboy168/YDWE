#pragma once

#include <base/config.h>
#include <Windows.h>			  		
#include <base/filesystem.h>

namespace base { namespace path { namespace detail {
	boost::filesystem::path quick_launch_path(bool default_user);
	boost::filesystem::path temp_path();
	boost::filesystem::path windows_path();
	boost::filesystem::path system_path();
	_BASE_API boost::filesystem::path module_path(HMODULE module_handle);
	_BASE_API boost::filesystem::path module_path(HANDLE process_handle, HMODULE module_handle);
}}}
