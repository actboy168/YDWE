#pragma once

#include <base/config.h>
#include <boost/filesystem.hpp>

namespace base {
namespace warcraft3 { namespace directory {

	_BASE_API bool read_current_user(boost::filesystem::path& result);
	_BASE_API bool read_all_users(boost::filesystem::path& result);
	_BASE_API bool validate(boost::filesystem::path const& p);
	_BASE_API bool write(boost::filesystem::path const& p);
	_BASE_API bool read(boost::filesystem::path& result);
	_BASE_API bool choose(const wchar_t* title, boost::filesystem::path& result);
	_BASE_API bool choose(const wchar_t* title);
	_BASE_API bool get(const wchar_t* title, boost::filesystem::path& war3_path);
}}
}
