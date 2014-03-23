#include <base/hook/iat.h>
#include <base/hook/detail/import_address_table.h>
#include <base/hook/detail/replace_pointer.h>

namespace base { namespace hook {
	uintptr_t iat(HMODULE module_handle, const char* dll_name, const char* api_name, uintptr_t new_function)
	{
		detail::import_address_table iat;

		if (!iat.open_module(module_handle))
		{
			return 0;
		}

		if (!iat.open_dll(dll_name))
		{
			return 0;
		}

		uintptr_t address = iat.get_proc_address(api_name);
		if (!address)
		{
			return 0;
		}

		return detail::replace_pointer(address, new_function);
	}

	uintptr_t iat(const wchar_t* module_name, const char* dll_name, const char* api_name, uintptr_t new_function)
	{
		return iat(::GetModuleHandleW(module_name), dll_name, api_name, new_function);
	}
}}
