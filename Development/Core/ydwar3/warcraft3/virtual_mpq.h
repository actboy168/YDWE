#pragma once

#include <warcraft3/config.h>			  		
#include <base/filesystem.h>
#include <functional>
#include <Windows.h>

namespace warcraft3::virtual_mpq {

	typedef std::function<bool(const std::string&, const void**, uint32_t*, uint32_t)> watch_cb;
	typedef std::function<void(const std::string&, const std::string&)> event_cb;
	typedef std::function<bool(const std::string&)> has_cb;

	_WAR3_API bool  initialize(HMODULE module_handle);
	_WAR3_API bool  open_path(const fs::path& p, uint32_t priority);
	_WAR3_API bool  close_path(const fs::path& p, uint32_t priority);
	_WAR3_API void* storm_alloc(size_t n);
	_WAR3_API void  watch(const std::string& filename, watch_cb callback);
	_WAR3_API void  force_watch(const std::string& filename, watch_cb callback);
	_WAR3_API void  map_load(watch_cb callback);
	_WAR3_API void  map_has(has_cb callback);
	_WAR3_API void  event(event_cb callback);
}
