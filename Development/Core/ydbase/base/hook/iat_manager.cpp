#include <base/hook/iat_manager.h>
#include <base/hook/detail/import_address_table.h>
#include <base/hook/detail/replace_pointer.h>
#include <cassert>

namespace base { namespace hook {
	iat_manager::iat_manager()
		: iat_ptr_(new detail::import_address_table())
	{ }

	iat_manager::~iat_manager()
	{
		release();
	}

	bool iat_manager::open_module(HMODULE module_handle)
	{
		return iat_ptr_->open_module(module_handle);
	}

	bool iat_manager::open_module(const wchar_t* module_name)
	{
		return iat_ptr_->open_module(::GetModuleHandleW(module_name));
	}

	bool iat_manager::open_dll(const char* dll_name)
	{
		return iat_ptr_->open_dll(dll_name);
	}

	bool iat_manager::hook(const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function)
	{
		if (!old_function_ptr)
			return false;

		uintptr_t address = iat_ptr_->get_proc_address(api_name);
		if (!address)
			return false;

		uintptr_t old_function = detail::replace_pointer(address, new_function);
		*old_function_ptr = old_function;
		hook_info_.push_front(hook_info(old_function, new_function, address));
		return true;
	}

	bool iat_manager::hook(const char* dll_name, const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function)
	{
		if (!open_dll(dll_name))
			return false;

		return hook(api_name, old_function_ptr, new_function);
	}

	bool iat_manager::hook(HMODULE module_handle, const char* dll_name, const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function)
	{
		if (!open_module(module_handle))
			return false;

		return hook(dll_name, api_name, old_function_ptr, new_function);
	}

	bool iat_manager::hook(const wchar_t* module_name, const char* dll_name, const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function)
	{
		if (!open_module(module_name))
			return false;

		return hook(dll_name, api_name, old_function_ptr, new_function);
	}

	bool iat_manager::unhook(uintptr_t old_function, uintptr_t new_function)
	{
		for (auto info = hook_info_.begin(); info != hook_info_.end(); ++info)
		{
			if ((info->old_function == old_function) && (info->new_function == new_function))
			{
				uintptr_t address = info->address;
				uintptr_t last_function = detail::replace_pointer(address, old_function);
				hook_info_.erase(info);
				if (!last_function)
				{
					return false;
				}
				assert(new_function == last_function);
				return true;
			}
		}

		return false;
	}

	void iat_manager::release()
	{
		for (auto info = hook_info_.begin(); info != hook_info_.end(); ++info)
		{
			uintptr_t last_function = detail::replace_pointer(info->address, info->old_function);

			if (last_function)
			{
				assert(info->new_function == last_function);
			}
		}
		hook_info_.clear();
	}
}}
