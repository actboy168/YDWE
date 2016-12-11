#pragma once

#include <base/config.h>
#include <Windows.h>
#include <cstdint>

namespace base {
namespace win {

	class _BASE_API pe_reader
	{	
	public:
		pe_reader();
		pe_reader(HMODULE module);
		void                  set_module(HMODULE module);
		HMODULE               module() const;
		size_t                module_size() const;
		uintptr_t             rva_to_addr(uintptr_t rva) const;
		PIMAGE_DOS_HEADER     get_dos_header() const;
		PIMAGE_NT_HEADERS     get_nt_headers() const;
		uintptr_t             get_directory_entry(uint32_t directory) const;
		uint32_t              get_directory_entry_size(uint32_t directory) const;
		uint32_t              get_num_sections() const;
		PIMAGE_SECTION_HEADER get_section_by_name(const char* section_name) const;
		
	private:
		uintptr_t module_;
	};
}

}
