#pragma once

#include <base/config.h>
#include <cstdint>

namespace base { namespace warcraft3 { namespace utility {

	struct objectid_64
	{
		uint32_t a;
		uint32_t b;

		inline objectid_64(uint32_t a_, uint32_t b_)
			: a(a_)
			, b(b_)
		{ }

		inline bool is_valid() const
		{
			return (a & b) != (uint32_t)(-1);
		}
	};

	_BASE_API uint32_t    objectid_64_to_32(const objectid_64& id);
	_BASE_API const char* get_class_name(uintptr_t ptr);
}}}
