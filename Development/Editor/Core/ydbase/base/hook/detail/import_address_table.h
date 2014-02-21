#pragma once

#include <base/win/pe_reader.h>
#include <Windows.h>
#include <cstdint>

namespace base { namespace hook { namespace detail {
	class import_address_table
	{
	public:
		import_address_table();

		bool open_module(HMODULE module_handle);
		bool open_dll(const char* dll_name);
		uintptr_t get_proc_address(const char* proc_name);

	private:
		uintptr_t get_proc_address_by_name(const char* proc_name);
		uintptr_t get_proc_address_by_ord(uintptr_t ordinal);

	private:
		win::pe_reader image_;
		PIMAGE_IMPORT_DESCRIPTOR cur_import_ptr_;
	};
}}}
