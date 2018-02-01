#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass/nf_register.h>
#include <base/warcraft3/version.h>
#include <base/hook/detail/replace_pointer.h>
#include <base/hook/iat.h>
#include <base/util/do_once.h>
#include <base/util/foreach.h>
#include <base/hook/fp_call.h>
#include <map>
#include <string>
#include <algorithm>

namespace base { 
namespace warcraft3 { namespace jass {

	bool table_hook     (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		if (!get_war3_searcher().get_instance(5))
			return false;

		if (!old_proc_ptr)
			return false;

		hashtable::native_func_node* node_ptr = get_native_function_hashtable()->get(proc_name);

		if (!node_ptr)
			return false;

		*old_proc_ptr = (uintptr_t)node_ptr->func_address_;
		node_ptr->func_address_  = (uint32_t)new_proc;

		return true;
	}

	bool table_unhook   (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t /*new_proc*/)
	{
		if (!old_proc_ptr)
			return false;

		hashtable::native_func_node* node_ptr = get_native_function_hashtable()->get(proc_name);

		if (!node_ptr)
			return false;

		node_ptr->func_address_ = (uint32_t)(*old_proc_ptr);

		return true;
	}

	bool register_hook  (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		if (!old_proc_ptr || !new_proc) 
			return false;

		uintptr_t hook_address = get_war3_searcher().search_string(proc_name);
		if (!hook_address)
			return false;

		uintptr_t old_proc = hook::detail::replace_pointer(hook_address + 0x05, new_proc);
		if (!old_proc)
			return false;

		*old_proc_ptr = old_proc;
		return true;
	}

	bool register_unhook(const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t /*new_proc*/)
	{
		if (!old_proc_ptr) 
			return false;

		uintptr_t hook_address = get_war3_searcher().search_string(proc_name);
		if (!hook_address)
			return false;

		hook::detail::replace_pointer(hook_address + 0x05, *old_proc_ptr);
		return true;
	}

	namespace detail {
		struct add_info
		{
			uintptr_t   func;
			std::string param;

			add_info()
				: func(0)
				, param()
			{ }

			void set(uintptr_t f, const char* p)
			{
				func = f;
				param = p;
			}
		};

		struct hook_info
		{
			uintptr_t*  old_proc_ptr;
			uintptr_t   new_proc;
			bool        japi;

			hook_info()
				: old_proc_ptr(0)
				, new_proc(0)
				, japi(false)
			{ }

			void set(uintptr_t* o, uintptr_t n, bool j)
			{
				old_proc_ptr = o;
				new_proc = n;
				japi = j;
			}
		};

		std::map<std::string, add_info>  add_info_list;
		std::map<std::string, add_info>  once_add_info_list;
		std::map<std::string, hook_info> hook_info_list;
		std::map<std::string, hook_info> once_hook_info_list;
		
		uintptr_t search_register_func()
		{
			uintptr_t ptr = get_war3_searcher().search_string("StringCase");
			ptr += 9;
			return convert_function(ptr);
		}

		void async_add(uintptr_t func, const char* name, const char* param)
		{
			add_info_list[name].set(func, param);
		}

		void async_once_add(uintptr_t func, const char* name, const char* param)
		{
			once_add_info_list[name].set(func, param);
		}

		void async_hook(const char* name, uintptr_t* old_proc_ptr, uintptr_t new_proc, bool japi)
		{
			hook_info_list[name].set(old_proc_ptr, new_proc, japi);
		}
		
		void async_unhook(const char* name)
		{
			hook_info_list.erase(name);
		}

		void async_once_hook(const char* name, uintptr_t* old_proc_ptr, uintptr_t new_proc, bool japi)
		{
			once_hook_info_list[name].set(old_proc_ptr, new_proc, japi);
		}

		void async_once_unhook(const char* name)
		{
			once_hook_info_list.erase(name);
		}

		void async_initialize()
		{
			nf_register::initialize();
		}
	}

	void nfunction_add()
	{
		japi_func_clean();
		static uintptr_t register_func = detail::search_register_func();
		foreach(auto const& it, detail::add_info_list)
		{
			fast_call<void>(register_func, it.second.func, it.first.c_str(), it.second.param.c_str());
			japi_func_add(it.first.c_str(), it.second.func);
		}
		foreach(auto const& it, detail::once_add_info_list)
		{
			fast_call<void>(register_func, it.second.func, it.first.c_str(), it.second.param.c_str());
			japi_func_add(it.first.c_str(), it.second.func);
		}
		detail::once_add_info_list.clear();
	}

	void nfunction_hook()
	{
		foreach(auto const& it, detail::hook_info_list)
		{
			table_hook(it.first.c_str(), it.second.old_proc_ptr, it.second.new_proc);
			if (it.second.japi) {
				japi_func_add(it.first.c_str(), it.second.new_proc);
			}
		}
		foreach(auto const& it, detail::once_hook_info_list)
		{
			table_hook(it.first.c_str(), it.second.old_proc_ptr, it.second.new_proc);
			if (it.second.japi) {
				japi_func_add(it.first.c_str(), it.second.new_proc);
			}
		}
		detail::once_hook_info_list.clear();
	}

	bool async_hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		detail::async_initialize();
		detail::async_hook(proc_name, old_proc_ptr, new_proc, false);
		return true;
	}

	bool async_once_hook       (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		detail::async_initialize();
		detail::async_once_hook(proc_name, old_proc_ptr, new_proc, false);
		return true;
	}

	bool async_once_unhook       (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		old_proc_ptr;
		new_proc;
		detail::async_initialize();
		detail::async_once_unhook(proc_name);
		return true;
	}

	bool async_unhook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		old_proc_ptr;
		new_proc;
		detail::async_initialize();
		detail::async_unhook(proc_name);
		return true;
	}

	bool japi_hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		detail::async_initialize();
		detail::async_hook(proc_name, old_proc_ptr, new_proc, true);
		return true;
	}

	bool japi_unhook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		old_proc_ptr;
		new_proc;
		detail::async_initialize();
		detail::async_unhook(proc_name);
		return true;
	}

	bool japi_once_hook(const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		detail::async_initialize();
		detail::async_once_hook(proc_name, old_proc_ptr, new_proc, true);
		return true;
	}

	uint32_t hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc, uint32_t flag)
	{
		uint32_t result = 0;

		if (flag & HOOK_MEMORY_TABLE)
		{
			if (table_hook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_MEMORY_TABLE;
			}
		}

		if (flag & HOOK_CODE_REGISTER)
		{
			if (register_hook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_MEMORY_TABLE;
			}
		}

		if (flag & HOOK_MEMORY_REGISTER)
		{
			if (async_hook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_MEMORY_TABLE;
			}
		}

		if (flag & HOOK_ONCE_MEMORY_REGISTER)
		{
			if (result == 0)
			{
				if (async_once_hook(proc_name, old_proc_ptr, new_proc))
				{
					result |= HOOK_ONCE_MEMORY_REGISTER;
				}
			}
			else
			{
				result |= HOOK_ONCE_MEMORY_REGISTER;
			}
		}

		return result;
	}

	uint32_t unhook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc, uint32_t flag)
	{
		uint32_t result = 0;

		if (flag & HOOK_MEMORY_TABLE)
		{
			if (table_unhook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_MEMORY_TABLE;
			}
		}

		if (flag & HOOK_CODE_REGISTER)
		{
			if (register_unhook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_MEMORY_TABLE;
			}
		}

		if (flag & HOOK_MEMORY_REGISTER)
		{
			if (async_unhook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_MEMORY_TABLE;
			}
		}
		
		if (flag & HOOK_ONCE_MEMORY_REGISTER)
		{
			if (async_once_unhook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_ONCE_MEMORY_REGISTER;
			}
		}

		return result;
	}

	bool japi_add            (uintptr_t func, const char* name, const char* param)
	{
		detail::async_initialize();
		detail::async_add(func, name, param);
		return true;
	}

	bool japi_once_add(uintptr_t func, const char* name, const char* param)
	{
		detail::async_initialize();
		detail::async_once_add(func, name, param);
		return true;
	}
}}
}
