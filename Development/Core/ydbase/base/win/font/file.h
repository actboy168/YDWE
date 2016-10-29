#pragma once

#include <base/config.h>
#include <string>				 	  		
#include <base/filesystem.h>

namespace base { namespace font {
	std::string get_name(const boost::filesystem::path& fil_path);
}}
