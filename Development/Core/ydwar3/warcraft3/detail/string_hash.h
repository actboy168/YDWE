#pragma once

#include <warcraft3/config.h>
#include <cstdint>

namespace warcraft3::detail {
	_WAR3_API uint32_t string_hash(const char* str);
}
