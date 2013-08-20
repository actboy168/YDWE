#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/warcraft3/detail/nf_register.h>
#include <ydwe/warcraft3/version.h>
#include <ydwe/hook/detail/replace_pointer.h>
#include <ydwe/hook/iat.h>
#include <ydwe/util/do_once.h>
#include <ydwe/util/foreach.h>
#include <aero/function/fp_call.hpp>
#include <vector>
#include <map>
#include <string>

_BASE_BEGIN namespace warcraft3 { namespace native_function {

	bool table_hook     (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
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

	bool hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		if (register_hook(proc_name, old_proc_ptr, new_proc))
		{
			if (table_hook(proc_name, old_proc_ptr, new_proc))
			{
				return true;
			}
			register_unhook(proc_name, old_proc_ptr, new_proc);
		}
		return false;
	}

	bool unhook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		bool b1 = register_unhook(proc_name, old_proc_ptr, new_proc);
		bool b2 = table_unhook   (proc_name, old_proc_ptr, new_proc);
		return b1 && b2;
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

	void async_add            (uintptr_t func, const char* name, const char* param)
	{
		detail::async_initialize();
		detail::async_add(func, name, param);
	}

	void async_hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		detail::async_initialize();
		detail::async_hook(proc_name, old_proc_ptr, new_proc);
	}

	namespace detail {
#pragma pack(push)
#pragma pack(1)
		struct asm_opcode_5
		{
			uint8_t  opcode;   
			uint32_t value;
		};

		// push    offset aRR      ; "(R)R"           68 3C D9 95 6F
		// mov     edx, offset aDeg2rad ; "Deg2Rad"   BA 34 D9 95 6F
		// mov     ecx, offset sub_6F3B3510           B9 10 35 3B 6F
		// call    sub_6F455C20                       E8 A7 10 08 00 
		struct asm_register_native_function
		{
		private:
			asm_opcode_5 Push;
			asm_opcode_5 MovEdx;
			asm_opcode_5 MovEcx;
			asm_opcode_5 Call;

		public:
			bool verify() const
			{
				return ((Push.opcode  == 0x68)
					&& (MovEdx.opcode == 0xBA)
					&& (MovEcx.opcode == 0xB9)
					&& (Call.opcode   == 0xE8));
			}

			const char* get_param() const
			{
				return (const char*)(Push.value);
			}

			const char* get_name() const
			{
				return (const char*)(MovEdx.value);
			}

			uintptr_t get_address() const
			{
				return (uintptr_t)(MovEcx.value);
			}
		};
#pragma pack(pop)
	}

	native_function::native_function()
		: return_(TYPE_NONE)
		, param_()
		, address_(0)
	{ }

	native_function::native_function(const char* param, uintptr_t address)
		: return_(TYPE_NONE)
		, param_()
		, address_(address)
	{
		if (!param || param[0] != '(')
		{
			return ;
		}

		const char* it = ++param;
		bool is_end = false;
		for (; *it; ++it)
		{
			if (*it == ')')
			{
				is_end = true;
			}
			else if (isupper(static_cast<unsigned char>(*it)))
			{
				if (is_end)
				{
					return_ = (variable_type)(*it);
					break;
				}
				else
				{
					param_.push_back((variable_type)(*it));
				}
			}
		}
	}

	native_function::native_function(native_function const& that, uintptr_t address)
		: return_(that.return_)
		, param_(that.param_)
		, address_(address)
	{ }

	bool native_function::is_valid() const
	{
		return return_ != TYPE_NONE;
	}

	std::vector<variable_type> const& native_function::get_param() const
	{
		return param_;
	}

	variable_type const& native_function::get_return() const
	{
		return return_;
	}

	uintptr_t native_function::get_address() const
	{
		return address_;
	}

	uintptr_t native_function::call(uintptr_t param_list[]) const
	{
		return jass::call(address_, param_list, param_.size());
	}

	class mapping : public std::map<std::string, native_function>
	{
	public:
		mapping()
		{ }

		static mapping initialize_from_register()
		{
			mapping m;

			uintptr_t ptr_Deg2Rad = get_war3_searcher().search_string("Deg2Rad");
			if (ptr_Deg2Rad)
			{
				for (detail::asm_register_native_function* ptr = (detail::asm_register_native_function*)(ptr_Deg2Rad - 6); ptr->verify(); ++ptr)
				{
					m.insert(std::make_pair(ptr->get_name(), native_function(ptr->get_param(), ptr->get_address())));
				}
			}

			return std::move(m);
		}
	};

	native_function const* jass_func(const char* proc_name)
	{
		static mapping m = mapping::initialize_from_register();

		auto it = m.find(proc_name);
		if (it != m.end() && it->second.is_valid())
		{
			return &(it->second);
		}

		return nullptr;
	}

	static mapping japi_mapping;

	native_function const* japi_func(const char* proc_name)
	{
		auto it = japi_mapping.find(proc_name);
		if (it != japi_mapping.end() && it->second.is_valid())
		{
			return &(it->second);
		}

		return nullptr;
	}

	void japi_add            (uintptr_t func, const char* name, const char* param)
	{
		async_add(func, name, param);
		japi_mapping.insert(std::make_pair(name, native_function(param, func)));
	}

	void japi_hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		async_hook(proc_name, old_proc_ptr, new_proc);
		native_function const* nf = jass_func(proc_name);
		if (nf)
		{
			japi_mapping.insert(std::make_pair(proc_name, native_function(*nf, new_proc)));
		}
	}
}}}
