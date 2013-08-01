#include <ydwe/warcraft3/basic_searcher.h>
#include <ydwe/warcraft3/detail/memory_search.h>

_BASE_BEGIN namespace warcraft3 {

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
}}
