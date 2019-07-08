#pragma once

#include <base/config.h>

namespace base {
	_BASE_API bool is_utf8(const char *source);
	_BASE_API bool is_utf8(const char *source, size_t length);
}
