#pragma once

#include <ydwe/config.h>
#include <cstdint>

namespace ydwe { namespace warcraft3 { namespace detail {
	YDWE_BASE_API uint32_t string_hash(const char* str);
}}}
