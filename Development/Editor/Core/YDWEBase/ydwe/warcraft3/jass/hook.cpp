#include <ydwe/warcraft3/jass/hook.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/warcraft3/jass/func_value.h>
#include <ydwe/warcraft3/jass/nf_register.h>
#include <ydwe/warcraft3/version.h>
#include <ydwe/hook/detail/replace_pointer.h>
#include <ydwe/hook/iat.h>
#include <ydwe/util/do_once.h>
#include <ydwe/util/foreach.h>
#include <aero/function/fp_call.hpp>
#include <vector>
#include <map>
#include <string>

_BASE_BEGIN 
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

		struct register_info
		{
			uintptr_t   func;
			std::string name; 
			std::string param;

			register_info(uintptr_t f, const char* n, const char* p)
				: func(f)
				, name(n)
				, param(p)
			{ }
		};

		struct hook_info
		{
			std::string proc_name;
			uintptr_t*  old_proc_ptr;
			uintptr_t   new_proc;

			hook_info(const char* p, uintptr_t* o, uintptr_t n)
				: proc_name(p)
				, old_proc_ptr(o)
				, new_proc(n)
			{ }
		};

		std::vector<register_info> register_info_list;
		std::vector<hook_info>     hook_info_list;

		uintptr_t search_register_func()
		{
			uintptr_t ptr = get_war3_searcher().search_string("StringCase");
			ptr += 9;
			return convert_function(ptr);
		}

		void async_add(uintptr_t func, const char* name, const char* param)
		{
			register_info_list.push_back(register_info(func, name, param));
		}

		void async_hook(const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
		{
			hook_info_list.push_back(hook_info(proc_name, old_proc_ptr, new_proc));
		}

		void async_initialize()
		{
			nf_register::initialize();

			DO_ONCE_NOTHREADSAFE()
			{
				nf_register::event_add.connect([&]()
				{
					static uintptr_t register_func = search_register_func();

					foreach (register_info const& it, register_info_list)
					{
						aero::fast_call<void>(register_func, it.func, it.name.c_str(), it.param.c_str());
					}
				});

				nf_register::event_hook.connect([&]()
				{
					foreach (hook_info const& it, hook_info_list)
					{
						table_hook(it.proc_name.c_str(), it.old_proc_ptr, it.new_proc);
					}
				});
			}
		}
	}

	bool async_hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		detail::async_initialize();
		detail::async_hook(proc_name, old_proc_ptr, new_proc);
		return true;
	}

	bool japi_hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		uint32_t flag  = HOOK_MEMORY_REGISTER | HOOK_AS_JAPI;
		uint32_t result = hook(proc_name, old_proc_ptr, new_proc, flag);
		return flag == result;
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

		if (flag & HOOK_MEMORY_REGISTER)
		{
			if (async_hook(proc_name, old_proc_ptr, new_proc))
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

		if (flag & HOOK_AS_JAPI)
		{
			if (japi_func_add(proc_name, new_proc))
			{
				result |= HOOK_AS_JAPI;
			}
		}

		return result;
	}

	uint32_t unhook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc, hook_type flag)
	{
		uint32_t result = 0;

		if (flag & HOOK_MEMORY_TABLE)
		{
			if (table_unhook(proc_name, old_proc_ptr, new_proc))
			{
				result |= HOOK_MEMORY_TABLE;
			}
		}

		if (flag & HOOK_MEMORY_REGISTER)
		{
			// FIXME
			//if (async_unhook(proc_name, old_proc_ptr, new_proc))
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

		if (flag & HOOK_AS_JAPI)
		{
			if (japi_func_remove(proc_name))
			{
				result |= HOOK_AS_JAPI;
			}
		}

		return result;
	}

	bool async_add            (uintptr_t func, const char* name, const char* param)
	{
		detail::async_initialize();
		detail::async_add(func, name, param);
		return true;
	}

	bool japi_add            (uintptr_t func, const char* name, const char* param)
	{
		return async_add(func, name, param)
			&& japi_func_add(name, func, param);
	}
}}
_BASE_END
