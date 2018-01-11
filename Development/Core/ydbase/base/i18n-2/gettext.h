#pragma once

#include <base/config.h>
#include <base/filesystem.h>
#include <string>

namespace base { namespace i18n { namespace v2 {
	_BASE_API void  initialize(const fs::path& p);
	_BASE_API bool  set_language(const std::wstring& l, bool refres);
	_BASE_API bool  set_domain(const std::wstring& d);
	_BASE_API const std::string&  get_text(const std::string& str);
	_BASE_API const std::wstring& get_language();
}}}
