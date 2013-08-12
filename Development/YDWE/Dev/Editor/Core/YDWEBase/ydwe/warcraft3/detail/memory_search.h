#pragma once

#include <cstdint>
#include <ydwe/config.h>

_BASE_BEGIN namespace warcraft3 { namespace detail {

	uintptr_t search_str_no_zero(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len);
	uintptr_t search_str(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len);
	uintptr_t search_int(uintptr_t beg, uintptr_t end, uint32_t val);
	_BASE_API uintptr_t convert_function(uintptr_t address);
	_BASE_API uintptr_t next_opcode(uintptr_t address, uint8_t opcode, size_t length);
	_BASE_API uintptr_t next_opcode(uintptr_t address, uint8_t opcode[], size_t n);
}}}
