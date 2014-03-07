#pragma once

#include <base/config.h>
#include <boost/filesystem.hpp>
#include <base/util/string_view.h>

namespace base { namespace i18n {
	_BASE_API bool              bindtextdomain(const char* domain, boost::filesystem::path const& mofile_path);
	_BASE_API const char*       textdomain(const char* domain);
	_BASE_API std::string_view gettext(const char* orig_str);
	_BASE_API std::string_view dgettext(const char* domain, const char* orig_str);
}}
