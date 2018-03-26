#pragma once

#include <base/filesystem.h>
#include <base/path/get_path.h>

// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace base { namespace path {
	inline fs::path self()
	{
		return std::move(module(reinterpret_cast<HMODULE>(&__ImageBase)));
	}
}}
