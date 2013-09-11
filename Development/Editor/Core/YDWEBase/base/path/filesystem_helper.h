#pragma once

#include <base/config.h>
#include <boost/filesystem.hpp>

_BASE_BEGIN namespace path {
	_BASE_API bool equal(boost::filesystem::path const& lhs, boost::filesystem::path const& rhs);
}}
