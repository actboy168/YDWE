#pragma once

#include <cstdint>
#include <ydwe/config.h>

_BASE_BEGIN namespace warcraft3 { namespace detail {

	uintptr_t search_str_no_zero(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len);
	uintptr_t search_str(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len);
	uintptr_t search_int(uintptr_t beg, uintptr_t end, uint32_t val);
}}}
