#pragma once

#include <base/config.h>		 	  		
#include <base/filesystem.h>

namespace base { namespace path {
	_BASE_API bool equal(boost::filesystem::path const& lhs, boost::filesystem::path const& rhs);
}}
