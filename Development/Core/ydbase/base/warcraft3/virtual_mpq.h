#pragma once

#include <base/config.h>			  		
#include <base/filesystem.h>
#include <Windows.h>

namespace base { namespace warcraft3 { namespace virtual_mpq {

	typedef std::function<bool(const std::string&, const void**, uint32_t*, uint32_t)> watch_cb;

	_BASE_API bool  initialize(HMODULE module_handle);
	_BASE_API bool  open_path(const fs::path& p, uint32_t priority);
	_BASE_API void* storm_alloc(size_t n);
	_BASE_API void  watch(const std::string& filename, watch_cb callback);
}}}
