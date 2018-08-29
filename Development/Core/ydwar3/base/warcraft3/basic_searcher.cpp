#include <base/warcraft3/basic_searcher.h>
extern "C" {
#include <base/warcraft3/udis86/udis86.h>
}

namespace base { namespace warcraft3 {
	namespace detail {
		static uintptr_t search_str_no_zero(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len)
		{
			for (uintptr_t ptr = beg; ptr < end - len; ++ptr)
			{
				if (0 == memcmp((const void*)ptr, str, len))
					return ptr;
			}

			return 0;
		}

		static uintptr_t search_str(uintptr_t beg, uintptr_t end, const char* str, uintptr_t len)
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

		static uintptr_t search_int(uintptr_t beg, uintptr_t end, uint32_t val)
		{
			for (uintptr_t ptr = beg; ptr < end - sizeof uint32_t; ++ptr)
			{
				if (0 == memcmp((const void*)ptr, &val, sizeof uint32_t))
					return ptr;
			}

			return 0;
		}
	}

	basic_searcher::basic_searcher(HMODULE hModule)
		: module_(hModule)
	{
		initialize();
	}

	void basic_searcher::initialize()
	{
		PIMAGE_SECTION_HEADER rdata_ptr = module_.get_section_by_name(".rdata");
		PIMAGE_SECTION_HEADER data_ptr  = module_.get_section_by_name(".data");
		PIMAGE_SECTION_HEADER text_ptr  = module_.get_section_by_name(".text");

		rdata_beg_ = module_.rva_to_addr(rdata_ptr->VirtualAddress);
		rdata_end_ = rdata_beg_ + rdata_ptr->SizeOfRawData;
		data_beg_  = module_.rva_to_addr(data_ptr->VirtualAddress);
		data_end_  = data_beg_ + data_ptr->SizeOfRawData;
		text_beg_  = module_.rva_to_addr(text_ptr->VirtualAddress);
		text_end_  = text_beg_ + text_ptr->SizeOfRawData;
	}

	uintptr_t basic_searcher::base() const
	{
		return (uintptr_t)module_.module();
	}

	size_t basic_searcher::size() const
	{
		return module_.module_size();
	}

	uintptr_t basic_searcher::search_string_ptr(const char* str, size_t length) const
	{
		uintptr_t retval;

		retval = detail::search_str(rdata_beg_, rdata_end_, str, length);
		if (retval)
			return retval;

		retval = detail::search_str(data_beg_, data_end_, str, length);
		if (retval)
			return retval;

		retval = detail::search_str_no_zero(rdata_beg_, rdata_end_, str, length);
		if (retval)
			return retval;

		retval = detail::search_str_no_zero(data_beg_, data_end_, str, length);
		if (retval)
			return retval;

		return 0;
	}

	uintptr_t basic_searcher::search_string(const char* str, size_t length) const
	{
		uintptr_t ptr = search_string_ptr(str, length);

		if (ptr)
		{
			return detail::search_int(text_beg_, text_end_, (uint32_t)ptr);
		}

		return 0;
	}

	uintptr_t basic_searcher::search_string(const char* str) const
	{
		return search_string(str, strlen(str)+1);
	}

	uintptr_t basic_searcher::search_string_part(const char* str) const
	{
		uintptr_t ptr = search_string_ptr(str, strlen(str) + 1);
		if (!ptr)
		{
			return 0;
		}
		for (;*(unsigned char*)ptr != ':';--ptr)
		{ }
		ptr--;
		return detail::search_int(text_beg_, text_end_, (uint32_t)ptr);
	}

	uintptr_t basic_searcher::search_int_in_text(uint32_t value) const
	{
		return detail::search_int(text_beg_, text_end_, (uint32_t)value);
	}

	uintptr_t basic_searcher::search_int_in_text(uint32_t value, uintptr_t beg) const
	{
		return detail::search_int(beg, text_end_, (uint32_t)value);
	}

	uintptr_t basic_searcher::search_int_in_rdata(uint32_t value) const
	{
		return detail::search_int(rdata_beg_, rdata_end_, (uint32_t)value);
	}

	uintptr_t basic_searcher::search_int_in_rdata(uint32_t value, uintptr_t beg) const
	{
		return detail::search_int(beg, rdata_end_, (uint32_t)value);
	}

	uintptr_t basic_searcher::search_int_in_data(uint32_t value) const
	{
		return detail::search_int(data_beg_, data_end_, (uint32_t)value);
	}

	uintptr_t basic_searcher::search_int_in_data(uint32_t value, uintptr_t beg) const
	{
		return detail::search_int(beg, data_end_, (uint32_t)value);
	}

	std::pair<uintptr_t, uintptr_t> basic_searcher::text() const
	{
		return std::make_pair(text_beg_, text_end_);
	}

	uintptr_t convert_function(uintptr_t address)
	{
		return address + *(uintptr_t*) (address+1) + 5;
	}

	uintptr_t next_opcode(uintptr_t address)
	{
		ud_t ud_obj;
		ud_init(&ud_obj, 32);
		ud_set_input_buffer(&ud_obj, (uint8_t*)address, (size_t)-1);
		return address + ud_decode(&ud_obj);
	}

	uintptr_t next_opcode(uintptr_t address, uint8_t opcode, size_t length)
	{
		ud_t ud_obj;
		ud_init(&ud_obj, 32);
		ud_set_input_buffer(&ud_obj, (uint8_t*)address, (size_t)-1);
		size_t size = 0;
		do
		{
			size = ud_decode(&ud_obj);
			if ((size == length) && (*(uint8_t*)address == opcode)) break;
			address += size;
		} while (size);

		return address;
	}
}}
