#pragma once

#include <base/config.h>
#include <Windows.h>
#include <list>
#include <memory>

namespace base { namespace hook {
	namespace detail
	{
		class import_address_table;
	}

	class _BASE_API iat_manager
	{
	protected:
		struct hook_info
		{
			hook_info(uintptr_t of, uintptr_t nf, uintptr_t a)
				: old_function(of)
				, new_function(nf)
				, address(a)
			{ }

			uintptr_t old_function;
			uintptr_t new_function;
			uintptr_t address;
		};

	public:
		iat_manager();
		virtual ~iat_manager();

		bool open_module(HMODULE module_handle);
		bool open_module(const wchar_t* module_name);
		bool open_dll(const char* dll_name);
		bool hook(const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function);
		bool hook(const char* dll_name, const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function);
		bool hook(HMODULE module_handle, const char* dll_name, const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function);
		bool hook(const wchar_t* module_name, const char* dll_name, const char* api_name, uintptr_t* old_function_ptr, uintptr_t new_function);
		bool unhook(uintptr_t old_function, uintptr_t new_function);
		void release();

	protected:
#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<detail::import_address_table> iat_ptr_;
		std::list<hook_info> hook_info_;
#pragma warning(pop)
	};
}}
