#pragma once

#include <warcraft3/config.h>			  		
#include <base/filesystem.h>

namespace warcraft3::directory {
	_WAR3_API bool read_current_user(fs::path& result);
	_WAR3_API bool read_all_users(fs::path& result);
	_WAR3_API bool validate(fs::path const& p);
	_WAR3_API bool write(fs::path const& p);
	_WAR3_API bool read(fs::path& result);
	_WAR3_API bool choose(const wchar_t* title, fs::path& result);
	_WAR3_API bool choose(const wchar_t* title);
	_WAR3_API bool get(const wchar_t* title, fs::path& war3_path);
}
