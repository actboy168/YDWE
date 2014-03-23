#include <base/warcraft3/detail/memory_search.h>
#include <cstring>

namespace base { namespace warcraft3 { namespace detail {

	uintptr_t search_str_no_zero(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len)
	{
		for (uintptr_t ptr = beg; ptr < end - len; ++ptr)
		{
			if (0 == memcmp((const void*)ptr, str, len))
				return ptr;
		}

		return 0;
	}

	uintptr_t search_str(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len)
	{
		for (uintptr_t ptr = beg; ptr < end - len; ++ptr)
		{
			if (*(uint8_t*)ptr == 0)
			{
				while (*(uint8_t*)ptr == 0) ptr++;

				if (0 == memcmp((const void*)ptr, str, len))
					return ptr;
			}
		}

		return 0;
	}

	uintptr_t search_int(uintptr_t beg, uintptr_t end, uint32_t val)
	{
		for (uintptr_t ptr = beg; ptr < end - sizeof uint32_t; ++ptr)
		{
			if (0 == memcmp((const void*)ptr, &val, sizeof uint32_t))
				return ptr;
		}

		return 0;
	}
}}}
