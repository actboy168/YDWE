#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/detail/memory_search.h>
#include <ydwe/warcraft3/version.h>
#include <ydwe/hook/detail/replace_pointer.h>
#include <ydwe/hook/iat.h>
#include <ydwe/util/foreach.h>
#include <aero/function/fp_call.hpp>
#include <vector>
#include <map>
#include <string>

namespace ydwe { namespace warcraft3 { namespace native_function {

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

		class nf_register
		{
		public:
			static void initialize()
			{
				static bool s_first = true;
				if (s_first)
				{
					s_first = false;

					register_func = search_register_func();
					
					if (get_war3_searcher().get_version() > version_123)
					{
						real_storm_alloc   = hook::iat(L"Game.dll", "Storm.dll",    (const char*)401, (uintptr_t)fake_storm_alloc);
						real_tls_get_value = hook::iat(L"Game.dll", "Kernel32.dll", "TlsGetValue",    (uintptr_t)fake_tls_get_value);
					}
					else if (get_war3_searcher().get_version() > version_121b)
					{
						real_storm_alloc   = hook::iat(L"Game.dll", "Storm.dll",    (const char*)401, (uintptr_t)fake_storm_alloc_122);
						real_tls_get_value = hook::iat(L"Game.dll", "Kernel32.dll", "TlsGetValue",    (uintptr_t)fake_tls_get_value);
					}
					else
					{
						real_storm_alloc   = hook::iat(L"Game.dll", "Storm.dll",    (const char*)401, (uintptr_t)fake_storm_alloc_120);
						real_tls_get_value = hook::iat(L"War3.exe", "Kernel32.dll", "TlsGetValue",    (uintptr_t)fake_tls_get_value);
					}
				}
			}

			static void add(uintptr_t func, const char* name, const char* param)
			{
				register_info_list.push_back(register_info(func, name, param));
			}

			static void hook(const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
			{
				hook_info_list.push_back(hook_info(proc_name, old_proc_ptr, new_proc));
			}

		private:
			static uintptr_t search_register_func()
			{
				uintptr_t ptr = get_war3_searcher().search_string("StringCase");
				ptr += 9;
				return warcraft3::detail::convert_function(ptr);
			}

			static void register_native_function()
			{
				foreach (register_info const& it, register_info_list)
				{
					aero::fast_call<void>(register_func, it.func, it.name.c_str(), it.param.c_str());
				}
			}

			static void hook_native_function()
			{
				foreach (hook_info const& it, hook_info_list)
				{
					table_hook(it.proc_name.c_str(), it.old_proc_ptr, it.new_proc);
				}
			}

			static uintptr_t __stdcall fake_storm_alloc(uint32_t amount, const char* log_filename, uint32_t log_line, uint32_t default_value)
			{
				if ((amount == 176) 
					&& (log_line == 668)
					&& (default_value == 0)
					&& (strcmp(log_filename, ".\\Jass.cpp") == 0))
				{
					stat = 1;
					thread_id = ::GetCurrentThreadId();
				}
				else if (stat == 3)
				{
					if (thread_id == ::GetCurrentThreadId())
					{
						if ((amount == 140) 
							&& (log_line == 90)
							&& (default_value == 0)
							&& (strcmp(log_filename, ".\\Agile.cpp") == 0))
						{
							stat = 0;
							hook_native_function();
						}
					}
				}

				return aero::std_call<uintptr_t>(real_storm_alloc, amount, log_filename, log_line, default_value);
			}

			static uintptr_t __stdcall fake_storm_alloc_122(uint32_t amount, const char* log_filename, uint32_t log_line, uint32_t default_value)
			{
				if ((amount == 176) 
					&& (log_line == 667)
					&& (default_value == 0)
					&& (strcmp(log_filename, ".\\Jass.cpp") == 0))
				{
					stat = 1;
					thread_id = ::GetCurrentThreadId();
				}
				else if (stat == 3)
				{
					if (thread_id == ::GetCurrentThreadId())
					{
						if ((amount == 140) 
							&& (log_line == 90)
							&& (default_value == 0)
							&& (strcmp(log_filename, ".\\Agile.cpp") == 0))
						{
							stat = 0;
							hook_native_function();
						}
					}
				}

				return aero::std_call<uintptr_t>(real_storm_alloc, amount, log_filename, log_line, default_value);
			}

			static uintptr_t __stdcall fake_storm_alloc_120(uint32_t amount, const char* log_filename, uint32_t log_line, uint32_t default_value)
			{
				if ((amount == 176) 
					&& (log_line == 667)
					&& (default_value == 0)
					&& (strcmp(log_filename, "E:\\Drive1\\temp\\buildwar3x\\engine\\source\\Jass2\\Jass.cpp") == 0))
				{
					stat = 1;
					thread_id = ::GetCurrentThreadId();
				}
				else if (stat == 3)
				{
					if (thread_id == ::GetCurrentThreadId())
					{
						if ((amount == 140) 
							&& (log_line == 90)
							&& (default_value == 0)
							&& (strcmp(log_filename, "E:\\Drive1\\temp\\buildwar3x\\engine\\Source\\Agile\\Agile.cpp") == 0))
						{
							stat = 0;
							hook_native_function();
						}
					}
				}

				return aero::std_call<uintptr_t>(real_storm_alloc, amount, log_filename, log_line, default_value);
			}

			static uintptr_t __stdcall fake_tls_get_value(uint32_t tls_index)
			{
				if ((stat != 0) && (thread_id == ::GetCurrentThreadId()))
				{
					if (stat == 1)
					{
						stat = 2;
					}
					else if (stat == 2)
					{
						stat = 3;
						register_native_function();
					}
				}

				return aero::std_call<uintptr_t>(real_tls_get_value, tls_index);
			}

		private:
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

			static uintptr_t                  thread_id;
			static uintptr_t                  stat;
			static uintptr_t                  real_storm_alloc;
			static uintptr_t                  real_tls_get_value;
			static uintptr_t                  register_func;
			static std::vector<register_info> register_info_list;
			static std::vector<hook_info>     hook_info_list;
		};

		uintptr_t                               nf_register::thread_id          = 0;
		uintptr_t                               nf_register::stat               = 0;
		uintptr_t                               nf_register::real_storm_alloc   = 0;
		uintptr_t                               nf_register::real_tls_get_value = 0;
		uintptr_t                               nf_register::register_func      = 0;
		std::vector<nf_register::register_info> nf_register::register_info_list;
		std::vector<nf_register::hook_info>     nf_register::hook_info_list;
	}

	void async_add            (uintptr_t func, const char* name, const char* param)
	{
		detail::nf_register::initialize();
		detail::nf_register::add(func, name, param);
	}

	void async_hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc)
	{
		detail::nf_register::initialize();
		detail::nf_register::hook(proc_name, old_proc_ptr, new_proc);
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
			else if (isupper(*it))
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
		uintptr_t func_address = address_;
		uintptr_t retval;
		uintptr_t esp_ptr;
		size_t  param_size = param_.size() * sizeof uintptr_t;

		_asm
		{
			sub  esp, param_size
			mov  esp_ptr, esp
		}
		memcpy((void*)esp_ptr, param_list, param_size);
		_asm
		{
			call [func_address]
			mov  esp, esp_ptr
			add  esp, param_size
			mov  retval, eax
		}

		return retval;
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
