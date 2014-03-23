#include <base/hook/eat.h>
#include <base/hook/detail/export_address_table.h>
#include <base/hook/detail/replace_pointer.h>

#include <Windows.h>

namespace base { namespace hook {

	uintptr_t eat(HMODULE module_handle, const char* api_name, uintptr_t new_function)
	{
		detail::export_address_table eat;

		if (!eat.open_module(module_handle))
		{
			return 0;
		}

		uintptr_t address = eat.get_proc_address(api_name);
		if (!address)
		{
			return 0;
		}

		return (uintptr_t)module_handle + (uintptr_t)detail::replace_pointer(address, (uintptr_t)new_function-(uintptr_t)module_handle);
	}

	uintptr_t eat(const wchar_t* module_name, const char* api_name, uintptr_t new_function)
	{
		HMODULE module_handle = ::GetModuleHandleW(module_name);
		if (!module_handle)
		{
			module_handle = ::LoadLibraryW(module_name);
			if (!module_handle)
			{
				return 0;
			}
		}
		return eat(module_handle, api_name, new_function);
	}
}}
