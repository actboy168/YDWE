#pragma once

#include <ydwe/config.h>
#include <boost/filesystem.hpp>

namespace ydwe { namespace path {
	YDWE_BASE_API bool equal(boost::filesystem::path const& lhs, boost::filesystem::path const& rhs);
}}
