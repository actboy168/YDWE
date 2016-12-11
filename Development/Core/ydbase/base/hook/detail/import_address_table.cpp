#include <base/hook/detail/import_address_table.h>
#include <cassert>

namespace base { namespace hook { namespace detail {

	import_address_table::import_address_table()
		: image_()
		, cur_import_ptr_(nullptr)
	{ }

	bool import_address_table::open_module(HMODULE module_handle)
	{
		image_.set_module(module_handle);
		return (module_handle != NULL);
	}

	bool import_address_table::open_dll(const char* dll_name)
	{
		assert(image_.module());

		PIMAGE_IMPORT_DESCRIPTOR import = (PIMAGE_IMPORT_DESCRIPTOR)image_.get_directory_entry(IMAGE_DIRECTORY_ENTRY_IMPORT);
		uint32_t size = image_.get_directory_entry_size(IMAGE_DIRECTORY_ENTRY_IMPORT);

		if (import == NULL || size < sizeof IMAGE_IMPORT_DESCRIPTOR)
			return false;

		for (; import->FirstThunk; ++import) 
		{
			if (0 == _stricmp((const char*)image_.rva_to_addr(import->Name), dll_name))
			{
				cur_import_ptr_ = import;
				return true;
			}
		}

		return false;
	}

	uintptr_t import_address_table::get_proc_address(const char* proc_name)
	{
		assert(image_.module());
		assert(cur_import_ptr_);
			
		if ((uintptr_t)proc_name >> 16)
		{
			return get_proc_address_by_name(proc_name);
		}
		else
		{
			return get_proc_address_by_ord((uintptr_t)proc_name);
		}
	}

	uintptr_t import_address_table::get_proc_address_by_name(const char* proc_name)
	{
		PIMAGE_THUNK_DATA pitd = (PIMAGE_THUNK_DATA)image_.rva_to_addr(cur_import_ptr_->OriginalFirstThunk);
		PIMAGE_THUNK_DATA pitd2 = (PIMAGE_THUNK_DATA)image_.rva_to_addr(cur_import_ptr_->FirstThunk);

		while (pitd->u1.Function)
		{
			PIMAGE_IMPORT_BY_NAME pi_import_by_name = (PIMAGE_IMPORT_BY_NAME)(image_.rva_to_addr(*(uintptr_t*)pitd));

			if (!IMAGE_SNAP_BY_ORDINAL(pitd->u1.Ordinal))
			{
				if (0 == strcmp((const char*)pi_import_by_name->Name, proc_name))
				{
					return (uintptr_t)(&pitd2->u1.Function);
				}
			}

			pitd++;
			pitd2++;
		}

		return 0;
	}

	uintptr_t import_address_table::get_proc_address_by_ord(uintptr_t ordinal)
	{
		PIMAGE_THUNK_DATA pitd = (PIMAGE_THUNK_DATA)image_.rva_to_addr(cur_import_ptr_->OriginalFirstThunk);
		PIMAGE_THUNK_DATA pitd2 = (PIMAGE_THUNK_DATA)image_.rva_to_addr(cur_import_ptr_->FirstThunk);

		while (pitd->u1.Function)
		{
			if (IMAGE_SNAP_BY_ORDINAL(pitd->u1.Ordinal))
			{
				if (pitd->u1.Ordinal == (ordinal | IMAGE_ORDINAL_FLAG))
				{
					return (uintptr_t)(&pitd2->u1.Function);
				}
			}

			pitd++;
			pitd2++;
		}
		return 0;
	}
}}}
