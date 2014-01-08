#pragma once

#include <base/config.h>
#include <cassert>
#include <cstdint>

namespace base { namespace hook {
	_BASE_API bool patch(uintptr_t address, const char* value, size_t value_size);


	template <size_t n>
	bool patch(uintptr_t address, const char(&value)[n])
	{
		assert(n > 1);
		return patch(address, value, n - 1);
	}

	template <size_t n>
	bool patch(uintptr_t address, char(&value)[n])
	{
		return patch(address, value, std::string::traits_type::length(value));
	}
}}
