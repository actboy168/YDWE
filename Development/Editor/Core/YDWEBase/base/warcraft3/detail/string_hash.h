#pragma once

#include <base/config.h>
#include <cstdint>

_BASE_BEGIN namespace warcraft3 { namespace detail {
	_BASE_API uint32_t string_hash(const char* str);
}}}
