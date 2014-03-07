#include <map>
#include <string>
#include <memory>
#include <base/hook/assembler/writer.h>
#include <base/util/noncopyable.h>
#include <base/util/horrible_cast.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/war3_searcher.h>
#include "../lua/jassbind.h"
#include "../lua/callback.h"
#include "libs_runtime.h"
#include "common.h"

#define CACHE_ALIGN __declspec(align(32))

namespace base { namespace warcraft3 { namespace lua_engine {

	int  jass_hook_real_function(lua_State* L);

	template <class T>
	struct aligned_delete
	{
		aligned_delete()
		{ }


#if _MSC_VER >= 1800
		template<class T2, class = typename std::enable_if<std::is_convertible<T2 *, T *>::value, void>::type>
#else
		template<class T2>
#endif
		aligned_delete(const aligned_delete<T2>&)
		{ }

		void operator()(T* ptr) const
		{
			static_assert(0 < sizeof (T),
			"can't delete an incomplete type");
			ptr->~T();
			_aligned_free(ptr);
		}
	};

	class jass_hook_helper : util::noncopyable
	{
		friend 	int jass_hook_real_function(lua_State* L);
	public:
		jass_hook_helper()
			: lj_(nullptr)
			, real_func_(0)
			, nf_(nullptr)
			, name_()
			, lua_fake_func_()
			, code_base_(0)
		{ }

		bool install(jassbind* lj, const jass::func_value* nf, const char* name, int lua_fake_func)
		{
			if (real_func_)
			{
				uninstall();
			}

			lj_ = lj;
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

		static jass_hook_helper* create()
		{
			jass_hook_helper* ptr = reinterpret_cast<jass_hook_helper*>(_aligned_malloc(sizeof (jass_hook_helper), 32));
			if (!ptr)
			{
				throw std::bad_alloc();
			}

			new(ptr)jass_hook_helper();
			return ptr;
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
			size_t param_size = nf_->get_param().size();

			for (size_t i = 0; i < param_size; ++i)
			{
				if (nf_->get_param()[i] == jass::TYPE_REAL)
				{
					lj_->push_real_precise(paramlist[i]? *(uintptr_t*)paramlist[i]: 0);
				}
				else
				{
					jass_push(lj_, nf_->get_param()[i], paramlist[i]);
				}
			}

			lj_->pushunsigned((uint32_t)(uintptr_t)this);
			lj_->pushcclosure((lua::state::cfunction)jass_hook_real_function, 1);

			return safe_call_ref(lj_, lua_fake_func_, param_size + 1, nf_->get_return());
		} 

	private:
		struct CACHE_ALIGN {
			hook::assembler::writer<32> jmp_code_;
#pragma warning(suppress:4201 4324)
		} ;

		jassbind*               lj_;           
		uintptr_t               real_func_;
		const jass::func_value* nf_;
		std::string             name_;
		int                     lua_fake_func_;
		uintptr_t               code_base_;
	};

	int jass_hook_real_function(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		jass_hook_helper* h = (jass_hook_helper*)(uintptr_t)lj->tounsigned(lua_upvalueindex(1));

		return jass_call_native_function(lj, h->nf_, h->real_func_);
	}

	std::map<std::string, std::unique_ptr<jass_hook_helper, aligned_delete<jass_hook_helper>>> g_hook_info_mapping;

	int install_jass_hook(jassbind* lj, const jass::func_value* nf, const char* name, int fake_func)
	{
		auto it = g_hook_info_mapping.find(name);
		if (it == g_hook_info_mapping.end())
		{
			g_hook_info_mapping[name].reset(jass_hook_helper::create());
		}

		g_hook_info_mapping[name]->install(lj, nf, name, fake_func);
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

	int jass_hook_set(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		const char* name = lj->tostring(2);

		const jass::func_value* nf = jass::jass_func(name);
		if (nf && nf->is_valid())
		{
			if (lj->isfunction(3))
			{
				install_jass_hook(lj, nf, name, runtime::callback_push(lj, 3));
			}
			else if (lj->isnil(3))
			{
				uninstall_jass_hook(lj, name);
			}
		}

		return 0;
	}

	int jass_hook_get(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		lj->pushnil();
		return 1;
	}

	int jass_hook(lua::state* ls)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushcclosure((lua::state::cfunction)jass_hook_get, 0);
				ls->rawset(-3);

				ls->pushstring("__newindex");
				ls->pushcclosure((lua::state::cfunction)jass_hook_set, 0);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);
		}
		return 1;
	}
}}}
