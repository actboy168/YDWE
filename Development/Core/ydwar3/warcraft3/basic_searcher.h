#pragma once

#include <warcraft3/config.h>
#include <base/win/pe_reader.h>
#include <utility>

namespace base { namespace warcraft3 {

	class _WAR3_API basic_searcher
	{
	public:
		basic_searcher(HMODULE hModule);
		uintptr_t base() const;
		size_t    size() const;
		uintptr_t search_string(const char* str, size_t length) const;
		uintptr_t search_string(const char* str) const;
		uintptr_t search_string_ptr(const char* str, size_t length) const;
		uintptr_t search_string_part(const char* str) const;

		uintptr_t search_int_in_text (uint32_t value) const;
		uintptr_t search_int_in_text (uint32_t value, uintptr_t beg) const;
		uintptr_t search_int_in_rdata(uint32_t value) const;
		uintptr_t search_int_in_rdata(uint32_t value, uintptr_t beg) const;
		uintptr_t search_int_in_data (uint32_t value) const;
		uintptr_t search_int_in_data (uint32_t value, uintptr_t beg) const;

		std::pair<uintptr_t, uintptr_t> text() const;

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

	_WAR3_API uintptr_t convert_function(uintptr_t address);
	_WAR3_API uintptr_t next_opcode(uintptr_t address);
	_WAR3_API uintptr_t next_opcode(uintptr_t address, uint8_t opcode, size_t length);
}}
