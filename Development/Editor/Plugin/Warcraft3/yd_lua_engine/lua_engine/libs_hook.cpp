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
#include "jassbind.h"
#include "callback.h"
#include "libs_runtime.h"
#include "common.h"

#define CACHE_ALIGN __declspec(align(32))

namespace base { namespace warcraft3 { namespace lua_engine { namespace hook {

	class jhook_helper : noncopyable
	{
	public:
		jhook_helper()
			: L_(nullptr)
			, nf_(nullptr)
			, name_()
			, real_func_(0)
			, fake_func_(LUA_REFNIL)
			, code_base_(0)
		{ }

		~jhook_helper()
		{
			uninstall();
		}

		bool install(lua_State* L, const jass::func_value* nf, const char* name, int fake_func)
		{
			if (real_func_)
			{
				uninstall();
			}

			L_ = get_mainthread(L);
			nf_ = nf;
			name_ = name;
			fake_func_ = fake_func;

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

		int pop_fake_function(lua_State* L) const
		{
			if (real_func_)
			{
				runtime::callback_read(L, fake_func_);
			}
			else
			{
				lua_pushnil(L);
			}
			return 1;
		}

		static jhook_helper* create()
		{
			jhook_helper* helper = reinterpret_cast<jhook_helper*>(_aligned_malloc(sizeof (jhook_helper), 32));
			if (!helper)
			{
				throw std::bad_alloc();
			}
			new(helper)jhook_helper();
			return helper;
		}

		static void destroy(jhook_helper* helper)
		{
			helper->~jhook_helper();
			_aligned_free(helper);
		}

	private:
		uintptr_t create_jmp_code()
		{
			jmp_code_.clear();

			uintptr_t code_base = (uintptr_t)jmp_code_.data();
			uintptr_t fake_func_addr = horrible_cast<uintptr_t>(&jhook_helper::fake_function);
			{
				using namespace base::hook::assembler;

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

		static int call_real_function(lua_State* L)
		{
			jhook_helper* helper = (jhook_helper*)(uintptr_t)lua_tointeger(L, lua_upvalueindex(1));
			return jass_call_native_function(L, helper->nf_, helper->real_func_);
		}

		uintptr_t fake_function(const uintptr_t* paramlist)
		{
			size_t param_size = nf_->get_param().size();

			for (size_t i = 0; i < param_size; ++i)
			{
				if (nf_->get_param()[i] == jass::TYPE_REAL)
				{
					jassbind::push_real_precise(L_, paramlist[i] ? *(uintptr_t*)paramlist[i] : 0);
				}
				else
				{
					jass_push(L_, nf_->get_param()[i], paramlist[i]);
				}
			}

			lua_pushinteger(L_, (uint32_t)(uintptr_t)this);
			lua_pushcclosure(L_, call_real_function, 1);

			return safe_call_ref(L_, fake_func_, param_size + 1, nf_->get_return());
		} 

	private:
		struct CACHE_ALIGN {
			base::hook::assembler::writer<32> jmp_code_;
#pragma warning(suppress:4201 4324)
		} ;

		lua_State*              L_;        
		const jass::func_value* nf_;
		std::string             name_;
		uintptr_t               real_func_;
		int                     fake_func_;
		uintptr_t               code_base_;
	};

#define LUA_JASS_HOOK "jhook_t"
	int install_jass_hook(lua_State* L, const jass::func_value* nf, const char* name, int fake_func)
	{
		runtime::get_global_table(L, "_JASS_HOOK_TABLE", false);
		lua_pushstring(L, name);
		lua_rawget(L, -2);

		jhook_helper* helper = 0;
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			helper = jhook_helper::create();
			intptr_t* ud = (intptr_t*)lua_newuserdata(L, sizeof(intptr_t));
			*ud = (intptr_t)helper;
			luaL_setmetatable(L, LUA_JASS_HOOK);

			lua_pushstring(L, name);
			lua_pushvalue(L, -2);
			lua_rawset(L, -4);
		}
		else
		{
			intptr_t* ud = (intptr_t*)lua_touserdata(L, -1);
			helper = (jhook_helper*)*ud;
		}
		helper->install(L, nf, name, fake_func);
		lua_pop(L, 1);
		return 0;
	}

	int uninstall_jass_hook(lua_State* L, const char* name)
	{
		runtime::get_global_table(L, "_JASS_HOOK_TABLE", false);
		lua_pushstring(L, name);
		lua_rawget(L, -2);

		if (!lua_isnil(L, -1))
		{
			intptr_t* ud = (intptr_t*)lua_touserdata(L, -1);
			jhook_helper* helper = (jhook_helper*)*ud;
			jhook_helper::destroy(helper);
		}
		lua_pop(L, 1);
		return 0;
	}

	int jass_hook_set(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		const jass::func_value* nf = jass::jass_func(name);
		if (nf && nf->is_valid())
		{
			if (lua_isfunction(L, 3))
			{
				install_jass_hook(L, nf, name, runtime::callback_push(L, 3));
			}
			else if (lua_isnil(L, 3))
			{
				uninstall_jass_hook(L, name);
			}
		}

		return 0;
	}

	int jass_hook_get(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);

		runtime::get_global_table(L, "_JASS_HOOK_TABLE", false);
		lua_pushstring(L, name);
		lua_rawget(L, -2);

		if (!lua_isnil(L, -1))
		{
			intptr_t* ud = (intptr_t*)lua_touserdata(L, -1);
			((jhook_helper*)*ud)->pop_fake_function(L);
		}
		return 1;
	}

	int jhook_gc(lua_State *L)
	{
		intptr_t* ud = (intptr_t*)lua_touserdata(L, 1);
		jhook_helper* helper = (jhook_helper*)*ud;
		jhook_helper::destroy(helper);
		return 0;
	}

	void jhook_make_mt(lua_State *L)
	{
		luaL_Reg lib[] = {
			{ "__gc", jhook_gc },
			{ NULL, NULL },
		};

		luaL_newmetatable(L, LUA_JASS_HOOK);
#if LUA_VERSION_NUM >= 502
		luaL_setfuncs(L, lib, 0);
		lua_pop(L, 1);
#else
		luaL_register(L, NULL, lib);
#endif
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, jass_hook_get, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, jass_hook_set, 0);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);
		}
		jhook_make_mt(L);
		return 1;
	}
}}}}
