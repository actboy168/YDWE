#pragma once

#include <base/config.h>
#include <set>
#include <string>

namespace base {  namespace font {
	_BASE_API size_t size_to_height(size_t size);
	_BASE_API size_t height_to_size(size_t height);
	_BASE_API std::set<std::wstring> get_list();
}}
