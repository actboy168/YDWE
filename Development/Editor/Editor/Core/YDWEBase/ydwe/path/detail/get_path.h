#pragma once

#include <ydwe/config.h>
#include <Windows.h>
#include <boost/filesystem.hpp>

namespace ydwe { namespace path { namespace detail {
	boost::filesystem::path GetQuickLaunchPath(bool default_user);
	boost::filesystem::path GetTempPath();
	boost::filesystem::path GetWindowsPath();
	boost::filesystem::path GetSystemPath();
	YDWE_BASE_API boost::filesystem::path GetModulePath(HMODULE module_handle);
}}}
