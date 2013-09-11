#include "../lua/callback.h"
#include "../lua/jassbind.h"
#include <base/warcraft3/jass/trampoline_function.h>
#include <Windows.h>

namespace base { namespace warcraft3 { namespace lua_engine {

	uintptr_t jass_read(jassbind* lj, jass::variable_type vt, int idx);

	lua::state*& instance()
	{
		static lua::state* s_state = nullptr;
		return s_state;
	}

	int safe_pcall (lua_State *pState, int nargs, int nresults)
	{
		int error = lua_pcall(pState, nargs, nresults, 0);
		switch (error)
		{
		case LUA_OK:
			break;
		case LUA_ERRRUN:
		case LUA_ERRMEM:
		case LUA_ERRERR:
			printf("Error(%d): %s\n", error, lua_tostring(pState, -1));
			lua_pop(pState, 1);
			break;
		default:
			printf("Error(%d)\n", error);
			break;
		}

		return error;
	}

	void do_buffer(const char* name, const char* buffer, size_t size)
	{
		lua::state*& ls = instance();
		if (luaL_loadbuffer(ls->self(), buffer, size, name) != LUA_OK)
		{
			printf("%s\n", ls->tostring(-1));
			ls->pop(1);
			return ;
		}

		safe_pcall(ls->self(), 0, 0);
	}

	callback::callback()
		: ref_(0)
	{ }

	callback::callback(lua::state* ls, uint32_t index)
		: ref_(0)
	{ 
		ls->pushvalue(index);
		ref_ = luaL_ref(ls->self(), LUA_REGISTRYINDEX);
	}

	callback::callback(uint32_t ref)
		: ref_(ref)
	{ }

	bool callback::call_pre() const
	{
		lua::state*& ls = instance();

		ls->rawgeti(LUA_REGISTRYINDEX, ref_);
		if (!ls->isfunction(-1))
		{
			printf("callback::call() attempt to call (not a function)\n");
			ls->pop(1);
			return false;
		}

		return true;
	}

	uintptr_t callback::call(size_t param_size, jass::variable_type result_vt) const
	{
		lua::state*& ls = instance();

		if (safe_pcall(ls->self(), param_size, (result_vt != jass::TYPE_NOTHING) ? 1: 0) != LUA_OK)
		{
			return 0;
		}
		
		if (result_vt == jass::TYPE_NOTHING)
		{
			return 0;
		}

		uintptr_t ret = jass_read((jassbind*)ls, result_vt, -1);
		ls->pop(1);
		return ret;
	}

	uint32_t __fastcall jass_callback(uint32_t param)
	{
		callback that(param);
		if (!that.call_pre())
		{
			return 0;
		}
 
		return that.call(0, jass::TYPE_BOOLEAN);
	}

	uint32_t cfunction_to_code(lua::state* ls, uint32_t index)
	{
		ls->pushvalue(index);
		return jass::trampoline_create(jass_callback, (uintptr_t)luaL_ref(ls->self(), LUA_REGISTRYINDEX));
	}
}}}
