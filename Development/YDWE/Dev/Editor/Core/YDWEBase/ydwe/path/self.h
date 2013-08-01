#pragma once

#include <boost/filesystem.hpp>
#include <ydwe/path/detail/get_path.h>

// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

_BASE_BEGIN namespace path {
	inline boost::filesystem::path self()
	{
		return std::move(detail::GetModulePath(reinterpret_cast<HMODULE>(&__ImageBase)));
	}
}}
