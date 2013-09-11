#include <map>
#include <string>
#include <aero/function/fp_call.hpp>
#include <base/hook/assembler/writer.h>
#include <base/util/noncopyable.h>
#include <base/util/horrible_cast.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/war3_searcher.h>
#include "../lua/jassbind.h"
#include "../lua/callback.h"

#define CACHE_ALIGN __declspec(align(32))

namespace base { namespace warcraft3 { namespace lua_engine {

	bool jass_push(jassbind* lj, jass::variable_type vt, uint32_t value);
	int  jass_call_native_function(jassbind* lj, const jass::func_value* nf, uintptr_t func_address = 0);
	int  jass_hook_real_function(lua_State* L);
	void install_on_trigger_event(jassbind* lj);

	class jass_hook_helper : util::noncopyable
	{
		friend 	int jass_hook_real_function(lua_State* L);
	public:
		jass_hook_helper()
			: real_func_(0)
			, nf_(nullptr)
			, name_()
			, lua_fake_func_()
			, code_base_(0)
		{ }

		bool install(const jass::func_value* nf, const char* name, const callback& lua_fake_func)
		{
			if (real_func_)
			{
				uninstall();
			}

			nf_ = nf;
			name_ = name;
			lua_fake_func_ = lua_fake_func;

			if (!code_base_)
			{
				code_base_ = create_jmp_code();
				if (!code_base_)
				{
					return false;
				}
			}

			return (jass::HOOK_MEMORY_TABLE | jass::HOOK_ONCE_MEMORY_REGISTER) == jass::hook(name_.c_str(), &real_func_, code_base_, jass::HOOK_MEMORY_TABLE | jass::HOOK_ONCE_MEMORY_REGISTER);
		}

		bool uninstall()
		{
			if (!real_func_)
			{
				return true;
			}

			if (jass::HOOK_MEMORY_TABLE & jass::unhook(name_.c_str(), &real_func_, code_base_, jass::HOOK_MEMORY_TABLE | jass::HOOK_ONCE_MEMORY_REGISTER))
			{
				return false;
			}

			real_func_ = 0;
			return true;
		}

	private:
		uintptr_t create_jmp_code()
		{
			jmp_code_.clear();

			uintptr_t code_base = (uintptr_t)jmp_code_.data();
			uintptr_t fake_func_addr = horrible_cast<uintptr_t>(&jass_hook_helper::fake_func);
			{
				using namespace hook::assembler;

				jmp_code_.push(esi);
				jmp_code_.mov(esi, esp);
				jmp_code_.add(operand(esi), 8);
				jmp_code_.push(esi);
				jmp_code_.mov(ecx, (uintptr_t)this);
				jmp_code_.call(fake_func_addr, code_base + jmp_code_.size());
				jmp_code_.pop(esi);
				jmp_code_.ret();
			}

			if (!jmp_code_.executable())
			{
				return 0;
			}

			return code_base;
		}

		uintptr_t fake_func(const uintptr_t* paramlist)
		{
			jassbind* lj = (jassbind*)instance();
			size_t param_size = nf_->get_param().size();

			if (!lua_fake_func_.call_pre())
			{
				return 0;
			}

			for (size_t i = 0; i < param_size; ++i)
			{
				if (nf_->get_param()[i] == jass::TYPE_REAL)
				{
					lj->push_real_precise(paramlist[i]? *(uintptr_t*)paramlist[i]: 0);
				}
				else
				{
					jass_push(lj, nf_->get_param()[i], paramlist[i]);
				}
			}

			lj->pushunsigned((uint32_t)(uintptr_t)this);
			lj->pushcclosure((lua::state::cfunction)jass_hook_real_function, 1);

			return lua_fake_func_.call(param_size+1, nf_->get_return());
		} 

	private:
		struct CACHE_ALIGN {
			hook::assembler::writer<32> jmp_code_;
#pragma warning(suppress:4201 4324)
		} ;

		uintptr_t               real_func_;
		const jass::func_value* nf_;
		std::string             name_;
		callback                lua_fake_func_;
		uintptr_t               code_base_;
	};

	int jass_hook_real_function(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		jass_hook_helper* h = (jass_hook_helper*)(uintptr_t)lj->tounsigned(lua_upvalueindex(1));

		return jass_call_native_function(lj, h->nf_, h->real_func_);
	}

	std::map<std::string, std::unique_ptr<jass_hook_helper>> g_hook_info_mapping;

	int install_jass_hook(jassbind*, const jass::func_value* nf, const char* name, const callback& fake_func)
	{
		auto it = g_hook_info_mapping.find(name);
		if (it == g_hook_info_mapping.end())
		{
			g_hook_info_mapping[name].reset(new jass_hook_helper());
		}

		g_hook_info_mapping[name]->install(nf, name, fake_func);
		return 0;
	}

	int uninstall_jass_hook(jassbind*, const char* name)
	{
		auto it = g_hook_info_mapping.find(name);
		if (it == g_hook_info_mapping.end())
		{
			return 0;
		}

		g_hook_info_mapping[name]->uninstall();
		return 0;
	}
}}}
