#include <base/hook/detail/export_address_table.h>

#include <cassert>

namespace base { namespace hook { namespace detail {

	export_address_table::export_address_table()
		: image_()
		, export_directory_ptr_(nullptr)
	{ }

	bool export_address_table::open_module(HMODULE module_handle)
	{
		image_.set_module(module_handle);
		if (image_.module())
		{
			export_directory_ptr_ = (PIMAGE_EXPORT_DIRECTORY)image_.get_directory_entry(IMAGE_DIRECTORY_ENTRY_EXPORT);
		}
		return (module_handle != NULL);
	}

	uintptr_t export_address_table::get_proc_address(const char* proc_name)
	{
		assert(image_.module());
		assert(export_directory_ptr_);

		if ((uintptr_t)proc_name >> 16)
		{
			return get_proc_address_by_name(proc_name);
		}
		else
		{
			return get_proc_address_by_ord((uintptr_t)proc_name);
		}
	}

	uintptr_t export_address_table::get_proc_address_by_name(const char* proc_name)
	{
		uint32_t* functions_address     = (uint32_t*)image_.rva_to_addr(export_directory_ptr_->AddressOfFunctions);
		uint32_t* names_address         = (uint32_t*)image_.rva_to_addr(export_directory_ptr_->AddressOfNames);
		uint16_t* name_ordinals_address = (uint16_t*)image_.rva_to_addr(export_directory_ptr_->AddressOfNameOrdinals);
		for (uint32_t i = 0; i < export_directory_ptr_->NumberOfNames; ++i)
		{
			const char* function_name = (const char*)image_.rva_to_addr(names_address[i]);
			if (0 == strcmp(function_name, proc_name))
			{
				return (uintptr_t)&(functions_address[name_ordinals_address[i]]);
			}
		}

		return 0;
	}

	uintptr_t export_address_table::get_proc_address_by_ord(uintptr_t ordinal)
	{
		if (ordinal < export_directory_ptr_->Base || ordinal > (export_directory_ptr_->Base + export_directory_ptr_->NumberOfFunctions - 1)) 
		{
			return 0;
		}

		uint32_t* functions_address     = (uint32_t*)image_.rva_to_addr(export_directory_ptr_->AddressOfFunctions);
		return (uintptr_t)&(functions_address[ordinal - export_directory_ptr_->Base]);
	}
}}}
