#include <base/win/pe_reader.h>

namespace base { namespace win {

	pe_reader::pe_reader() 
		: module_(0) 
	{ }

	pe_reader::pe_reader(HMODULE module) 
		: module_((uintptr_t)module) 
	{ }

	void pe_reader::set_module(HMODULE module)
	{
		module_ = (uintptr_t)module;
	}

	HMODULE pe_reader::module() const 
	{ 
		return (HMODULE)(module_);
	}

	size_t pe_reader::module_size() const
	{
		return get_nt_headers()->OptionalHeader.SizeOfImage + get_nt_headers()->OptionalHeader.SizeOfHeaders;
	}

	uintptr_t pe_reader::rva_to_addr(uintptr_t rva) const
	{
		if (rva == 0) return 0;
		return (uintptr_t)(module_) + rva;
	}

	PIMAGE_DOS_HEADER pe_reader::get_dos_header() const 
	{
		return (PIMAGE_DOS_HEADER)(module_);
	}

	PIMAGE_NT_HEADERS pe_reader::get_nt_headers() const
	{
		PIMAGE_DOS_HEADER dos_header = get_dos_header();
		return (PIMAGE_NT_HEADERS)((uintptr_t)(dos_header) + dos_header->e_lfanew);
	}

	uintptr_t pe_reader::get_directory_entry(uint32_t directory) const
	{
		PIMAGE_NT_HEADERS nt_headers = get_nt_headers();
		return rva_to_addr(nt_headers->OptionalHeader.DataDirectory[directory].VirtualAddress);
	}

	uint32_t pe_reader::get_directory_entry_size(uint32_t directory) const 
	{
		PIMAGE_NT_HEADERS nt_headers = get_nt_headers();
		return nt_headers->OptionalHeader.DataDirectory[directory].Size;
	}

	uint32_t pe_reader::get_num_sections() const 
	{
		return get_nt_headers()->FileHeader.NumberOfSections;
	}

	PIMAGE_SECTION_HEADER pe_reader::get_section_by_name(const char* section_name) const 
	{
		uint32_t num_sections = get_num_sections();

		PIMAGE_SECTION_HEADER section_ptr = IMAGE_FIRST_SECTION(get_nt_headers());
		for (uint32_t i = 0; i < num_sections; ++i, ++section_ptr)
		{
			if (0 == _strnicmp((const char*)(section_ptr->Name), section_name, sizeof section_ptr->Name)) 
			{
				return section_ptr;
			}
		}

		return nullptr;
	}
}}
