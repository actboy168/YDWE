#pragma once

#include <base/config.h>			  		
#include <base/filesystem.h>

namespace base {
namespace warcraft3 { namespace directory {

	_BASE_API bool read_current_user(fs::path& result);
	_BASE_API bool read_all_users(fs::path& result);
	_BASE_API bool validate(fs::path const& p);
	_BASE_API bool write(fs::path const& p);
	_BASE_API bool read(fs::path& result);
	_BASE_API bool choose(const wchar_t* title, fs::path& result);
	_BASE_API bool choose(const wchar_t* title);
	_BASE_API bool get(const wchar_t* title, fs::path& war3_path);
}}
}
