#pragma once

#include <base/config.h>
#include <string>
#include <boost/filesystem.hpp>

namespace base { namespace font {
	std::string get_name(const boost::filesystem::path& fil_path);
}}
