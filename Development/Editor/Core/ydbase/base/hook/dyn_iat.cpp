#include <base/hook/dyn_iat.h>
#include <base/hook/fp_call.h>
#include <base/hook/iat.h>
#include <Windows.h>
#include <map>
#include <string>

namespace base { namespace hook {

	namespace real
	{
		uintptr_t getprocaddress = 0;
	}

	namespace fake
	{
		struct hookfunction_t
		{
			uintptr_t  new_func;
			uintptr_t* old_func;

			void add_funtion(uintptr_t* old_function, uintptr_t new_function)
			{
				new_func = new_function;
				old_func = old_function;
			}

			uintptr_t try_hook(uintptr_t old_funcion)
			{
				if (old_func)
				{
					*old_func = old_funcion;
				}
				return new_func;
			}
		};

		struct dllfunction_t
		{
			std::map<std::string, hookfunction_t> func_by_name;
			std::map<uintptr_t, hookfunction_t>   func_by_ord;

			void add_funtion(const char* api_name, uintptr_t* old_function, uintptr_t new_function)
			{
				if ((uintptr_t)api_name >> 16)
				{
					func_by_name[api_name].add_funtion(old_function, new_function);
				}
				else
				{
					func_by_ord[(uintptr_t)api_name].add_funtion(old_function, new_function);
				}
			}

			uintptr_t try_hook(const char* api_name, uintptr_t old_funcion)
			{
				if ((uintptr_t)api_name >> 16)
				{
					auto it = func_by_name.find(api_name);
					if (it != func_by_name.end())
					{
						return it->second.try_hook(old_funcion);
					}
				}
				else
				{
					auto it = func_by_ord.find((uintptr_t)api_name);
					if (it != func_by_ord.end())
					{
						return it->second.try_hook(old_funcion);
					}
				}
				return old_funcion;
			}
		};

		struct allfunction_t
			: public std::map<std::wstring, dllfunction_t>
		{
			void add_funtion(const wchar_t* dll_name, const char* api_name, uintptr_t* old_function, uintptr_t new_function)
			{
				(*this)[dll_name].add_funtion(api_name, old_function, new_function);
			}

			uintptr_t try_hook(HMODULE dll_handle, const char* api_name, uintptr_t old_funcion)
			{
				for (auto it = begin(); it != end(); ++it)
				{
					if (dll_handle == GetModuleHandleW(it->first.c_str()))
					{
						uintptr_t result = it->second.try_hook(api_name, old_funcion);
						if (result)
						{
							return result;
						}
					}
				}
				return old_funcion;
			}
		};

		allfunction_t dyniatfunc;

		uintptr_t __stdcall getprocaddress(HMODULE lib, const char* name)
		{
			uintptr_t old_funcion = std_call<uintptr_t>(real::getprocaddress, lib, name);
			return dyniatfunc.try_hook(lib, name, old_funcion);
		}
	}

	bool dyn_iat(HMODULE module_handle, const wchar_t* dll_name, const char* api_name, uintptr_t* old_function, uintptr_t new_function)
	{
		fake::dyniatfunc.add_funtion(dll_name, api_name, old_function, new_function);

		if (real::getprocaddress == 0)
		{
			real::getprocaddress = iat(module_handle, "kernel32.dll", "GetProcAddress", (uintptr_t)fake::getprocaddress);
		}

		return true;
	}

	bool dyn_iat(const wchar_t* module_name, const wchar_t* dll_name, const char* api_name, uintptr_t* old_function, uintptr_t new_function)
	{
		return dyn_iat(::GetModuleHandleW(module_name), dll_name, api_name, old_function, new_function);
	}
}}
