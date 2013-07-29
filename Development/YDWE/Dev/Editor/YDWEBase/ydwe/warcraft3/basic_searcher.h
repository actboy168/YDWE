#pragma once

#include <ydwe/config.h>
#include <ydwe/win/pe_reader.h>

namespace ydwe { namespace warcraft3 {

	class YDWE_BASE_API basic_searcher
	{
	public:
		basic_searcher(HMODULE hModule);
		uintptr_t search_string(const char* str, size_t length) const;
		uintptr_t search_string(const char* str) const;
		uintptr_t search_string_ptr(const char* str, size_t length) const;
		uintptr_t search_int(uint32_t value) const;
		uintptr_t search_int(uint32_t value, uintptr_t beg) const;

		uintptr_t search_int_in_text (uint32_t value) const;
		uintptr_t search_int_in_text (uint32_t value, uintptr_t beg) const;
		uintptr_t search_int_in_rdata(uint32_t value) const;
		uintptr_t search_int_in_rdata(uint32_t value, uintptr_t beg) const;
		uintptr_t search_int_in_data (uint32_t value) const;
		uintptr_t search_int_in_data (uint32_t value, uintptr_t beg) const;

	private:
		void initialize();

	private:
		win::pe_reader module_;
		uintptr_t rdata_beg_;
		uintptr_t rdata_end_;
		uintptr_t data_beg_;
		uintptr_t data_end_;
		uintptr_t text_beg_;
		uintptr_t text_end_;
	};
}}
