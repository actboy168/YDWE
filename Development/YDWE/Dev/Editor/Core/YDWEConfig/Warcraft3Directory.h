#pragma once

#include <boost/filesystem.hpp>

namespace warcraft3_directory
{
	bool read_current_user(boost::filesystem::path& result);
	bool read_all_users(boost::filesystem::path& result);
	bool validate(boost::filesystem::path const& p);
	bool write(boost::filesystem::path const& p);
	bool read(boost::filesystem::path& result);
	bool choose(const wchar_t* title, boost::filesystem::path& result);
	bool choose(const wchar_t* title);
}
