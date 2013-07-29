#include <ydwe/warcraft3/detail/memory_search.h>
#include <ydwe/hook/detail/disassembly.h>
#include <cstring>

namespace ydwe { namespace warcraft3 { namespace detail {

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

	uintptr_t convert_function(uintptr_t address)
	{
		return address + *(uintptr_t*) (address+1) + 5;
	}

	uintptr_t next_opcode(uintptr_t address, uint8_t opcode, size_t length)
	{
		size_t size = 0;
		do
		{
			uint8_t* op = nullptr;
			size = hook::detail::next_opcode(address, &op);
			if ((size == length) && (op[0] == opcode)) break;
			address += size;
		} while (size);

		return address;
	}
}}}
