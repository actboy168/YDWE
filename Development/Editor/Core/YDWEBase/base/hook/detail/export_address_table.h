#pragma once

#include <base/win/pe_reader.h>
#include <Windows.h>

namespace base { namespace hook { namespace detail {
	class export_address_table
	{
	public:
		export_address_table();
		bool open_module(HMODULE module_handle);
		uintptr_t get_proc_address(const char* proc_name);

	private:
		uintptr_t get_proc_address_by_name(const char* proc_name);
		uintptr_t get_proc_address_by_ord(uintptr_t ordinal);

	private:
		win::pe_reader image_;
		PIMAGE_EXPORT_DIRECTORY export_directory_ptr_;
	};
}}}
