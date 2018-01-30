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

namespace base { namespace warcraft3 { namespace lua_engine { namespace hook {

	template <class Base, int(Base::* extFunc)(lua_State*)>
	struct lua_to_nativefunction : noncopyable
	{
		lua_to_nativefunction(lua_State* L, int luaobj, const jass::func_value* funcdef, Base* base)
			: L(get_mainthread(L))
			, funcdef(funcdef)
			, luaobj(luaobj)
			, funcentry((uintptr_t)code.data())
			, base(base)
		{
			using namespace base::hook::assembler;
			code.push(esi);
			code.mov(esi, esp);
			code.add(operand(esi), 8);
			code.push(esi);
			code.mov(ecx, (uintptr_t)this);
			code.call(horrible_cast<uintptr_t>(&lua_to_nativefunction::lua_function), funcentry + code.size());
			code.pop(esi);
			code.ret();
			if (!code.executable()) {
				throw std::bad_alloc();
			}
		}

		uintptr_t lua_function(const uintptr_t* paramlist)
		{
			size_t param_size = funcdef->get_param().size();

			for (size_t i = 0; i < param_size; ++i)
			{
				if (funcdef->get_param()[i] == jass::TYPE_REAL)
				{
					jassbind::push_real_precise(L, paramlist[i] ? *(uintptr_t*)paramlist[i] : 0);
				}
				else
				{
					jass_push(L, funcdef->get_param()[i], paramlist[i]);
				}
			}
			int n = (base->*extFunc)(L);
			return safe_call_ref(L, luaobj, param_size + n, funcdef->get_return());
		}

		struct __declspec(align(32)) {
			::base::hook::assembler::writer<32> code;
#pragma warning(suppress:4201 4324)
		};
		lua_State*              L;
		int                     luaobj;
		const jass::func_value* funcdef;
		uintptr_t               funcentry;
		Base*                   base;
	};

	class jhook_t
	{
	public:
		jhook_t()
			: base_(nullptr)
			, name_()
			, real_func_(0)
		{ }

		~jhook_t()
		{
			uninstall();
		}

		bool install(lua_State* L, const jass::func_value* funcdef, const char* name, int luaobj)
		{
			if (base_)
			{
				if (!uninstall()) {
					return false;
				}
			}
			name_ = name;
			base_ = new nfunction(L, luaobj, funcdef, this);
			return (jass::HOOK_MEMORY_TABLE | jass::HOOK_ONCE_MEMORY_REGISTER) == jass::hook(name_.c_str(), &real_func_, base_->funcentry, jass::HOOK_MEMORY_TABLE | jass::HOOK_ONCE_MEMORY_REGISTER);
		}

		bool uninstall()
		{
			if (!base_)
			{
				return true;
			}
			if (jass::HOOK_MEMORY_TABLE & jass::unhook(name_.c_str(), &real_func_, base_->funcentry, jass::HOOK_MEMORY_TABLE | jass::HOOK_ONCE_MEMORY_REGISTER))
			{
				return false;
			}
			delete base_;
			base_ = nullptr;
			return true;
		}

		int push_luaobj(lua_State* L) const
		{
			if (base_)
			{
				runtime::callback_read(L, base_->luaobj);
			}
			else
			{
				lua_pushnil(L);
			}
			return 1;
		}

		static int call_real_function(lua_State* L)
		{
			jhook_t* self = (jhook_t*)(uintptr_t)lua_tointeger(L, lua_upvalueindex(1));
			return jass_call_native_function(L, self->base_->funcdef, self->real_func_);
		}

		int push_real_function(lua_State* L)
		{
			lua_pushinteger(L, (uint32_t)(uintptr_t)this);
			lua_pushcclosure(L, call_real_function, 1);
			return 1;
		}

		typedef lua_to_nativefunction<jhook_t, &jhook_t::push_real_function> nfunction;

	private:
		nfunction*  base_;
		std::string name_;
		uintptr_t   real_func_;
	};

#define LUA_JASS_HOOK "jhook_t"

	static jhook_t* get_jhook(lua_State* L, const char* name)
	{
		runtime::get_global_table(L, "_JASS_HOOK_TABLE", false);
		lua_pushstring(L, name);
		lua_rawget(L, -2);
		return (jhook_t*)luaL_testudata(L, -1, LUA_JASS_HOOK);
	}

	static int mt_newindex(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 2);
		const jass::func_value* funcdef = jass::jass_func(name);
		if (!funcdef || !funcdef->is_valid()) {
			return 0;
		}

		jhook_t* self = get_jhook(L, name);
		if (self) {
			self->uninstall();
		}

		if (lua_isfunction(L, 3)) {
			if (!self) {
				runtime::get_global_table(L, "_JASS_HOOK_TABLE", false);
				lua_pushstring(L, name);
				self = (jhook_t*)lua_newuserdata(L, sizeof(jhook_t));
				new (self)jhook_t;
				luaL_setmetatable(L, LUA_JASS_HOOK);
				lua_rawset(L, -3);
			}

			int luaobj = runtime::callback_push(L, 3);
			self->install(L, funcdef, name, luaobj);
		}
		return 0;
	}

	static int mt_index(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 2);
		jhook_t* self = get_jhook(L, name);
		if (self) {
			return self->push_luaobj(L);
		}
		lua_pushnil(L);
		return 1;
	}

	static int mt_gc(lua_State *L)
	{
		jhook_t* self = (jhook_t*)luaL_testudata(L, 1, LUA_JASS_HOOK);
		if (self) {
			self->~jhook_t();
		}
		return 0;
	}

	int open(lua_State* L)
	{
		luaL_Reg mt1[] = {
			{ "__gc", mt_gc },
			{ NULL, NULL },
		};

		luaL_newmetatable(L, LUA_JASS_HOOK);
		luaL_setfuncs(L, mt1, 0);
		lua_pop(L, 1);

		lua_newtable(L);
		luaL_Reg mt2[] = {
			{ "__index", mt_index },
			{ "__newindex", mt_newindex },
			{ NULL, NULL },
		};
		luaL_newlib(L, mt2);
		lua_setmetatable(L, -2);
		return 1;
	}
}}}}
