#include "../lua/callback.h"

namespace ydwe { namespace warcraft3 { namespace lua_engine { namespace lua {

	state*& instance()
	{
		static state* s_state = nullptr;
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
			break;
		default:
			printf("Error(%d)\n", error);
			break;
		}

		return error;
	}

	void do_buffer(const char* name, const char* buffer, size_t size)
	{
		state*& ls = instance();
		if (luaL_loadbuffer(ls->self(), buffer, size, name) != LUA_OK)
		{
			printf("%s\n", ls->tostring(-1));
			ls->pop(1);
			return ;
		}

		if (safe_pcall(ls->self(), 0, 0) != LUA_OK)
		{
			ls->pop(1);
		}
	}

	callback::callback()
		: ref_(0)
	{ }

	callback::callback(lua::state* ls, uint32_t index)
		: ref_(create(ls, index))
	{ }

	callback::callback(uint32_t ref)
		: ref_(ref)
	{ }

	callback::operator uint32_t()
	{
		return ref_;
	}

	int callback::create(lua::state* ls, uint32_t index)
	{
		ls->pushvalue(index);
		return luaL_ref(ls->self(), LUA_REGISTRYINDEX);
	}

	bool callback::call_pre() const
	{
		state*& ls = instance();

		ls->rawgeti(LUA_REGISTRYINDEX, ref_);
		if (!ls->isfunction(-1))
		{
			printf("callback::call() attempt to call (not a function)\n");
			ls->pop(1);
			return false;
		}

		return true;
	}

	uint32_t callback::call(int nargs, bool result) const
	{
		state*& ls = instance();

		if (safe_pcall(ls->self(), nargs, result? 1: 0) != LUA_OK)
		{
			ls->pop(1);
			ls->pop(1);
			return 0;
		}

		uint32_t retval = 0;

		if (result)
		{
			if (!ls->isnil(-1))
			{
				if (ls->isnumber(-1))
				{
					retval = (uint32_t)ls->tonumber(-1);
				}

				if (ls->isboolean(-1))
				{
					retval = ls->toboolean(-1) ? 1 : 0;
				}
			}
			ls->pop(1);
		}

		ls->pop(1);
		return retval;
	}

}}}}
