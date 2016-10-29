#pragma once

#include <base/config.h>
#include <string>				 	  		
#include <base/filesystem.h>

namespace base { namespace font {
	std::string get_name(const fs::path& fil_path);
}}
