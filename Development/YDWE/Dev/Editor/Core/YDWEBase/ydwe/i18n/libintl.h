#pragma once

#include <ydwe/config.h>
#include <boost/filesystem.hpp>

_BASE_BEGIN namespace i18n {
	_BASE_API bool        bindtextdomain(const char* domain, boost::filesystem::path const& mofile_path);
	_BASE_API const char* textdomain(const char* domain);
	_BASE_API const char* gettext(const char* orig_str);
	_BASE_API const char* dgettext(const char* domain, const char* orig_str);
}}
