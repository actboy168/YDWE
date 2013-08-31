#include <ydwe/warcraft3/jass/func_value.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/jass.h>
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

	func_value::func_value()
		: return_(TYPE_NONE)
		, param_()
		, address_(0)
	{ }

	func_value::func_value(const char* param, uintptr_t address)
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

	func_value::func_value(func_value const& that, uintptr_t address)
		: return_(that.return_)
		, param_(that.param_)
		, address_(address)
	{ }

	bool func_value::is_valid() const
	{
		return return_ != TYPE_NONE;
	}

	std::vector<variable_type> const& func_value::get_param() const
	{
		return param_;
	}

	variable_type const& func_value::get_return() const
	{
		return return_;
	}

	uintptr_t func_value::get_address() const
	{
		return address_;
	}

	uintptr_t func_value::call(uintptr_t param_list[]) const
	{
		return jass::call(address_, param_list, param_.size());
	}

	class mapping : public std::map<std::string, func_value>
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
					m.insert(std::make_pair(ptr->get_name(), func_value(ptr->get_param(), ptr->get_address())));
				}
			}

			return std::move(m);
		}
	};

	func_value const* jass_func(const char* proc_name)
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

	func_value const* japi_func(const char* proc_name)
	{
		auto it = japi_mapping.find(proc_name);
		if (it != japi_mapping.end() && it->second.is_valid())
		{
			return &(it->second);
		}

		return nullptr;
	}
	
	bool japi_func_add(const char* proc_name, uintptr_t new_proc)
	{
		func_value const* nf = jass_func(proc_name);
		if (!nf)
		{
			return false;
		}

		return japi_mapping.insert(std::make_pair(proc_name, func_value(*nf, new_proc))).second;
	}

	bool japi_func_add(const char* proc_name, uintptr_t new_proc, const char* param)
	{
		return japi_mapping.insert(std::make_pair(proc_name, func_value(param, new_proc))).second;
	}

	bool japi_func_remove(const char* proc_name)
	{
		auto it = japi_mapping.find(proc_name);
		if (it != japi_mapping.end())
		{
			japi_mapping.erase(it);
			return true;
		}

		return false;
	}
}}
_BASE_END
