#include "callback.h"
#include "jassbind.h"
#include "libs_runtime.h"
#include "common.h"
#include <base/warcraft3/jass/trampoline_function.h>
#include <Windows.h>
#include <cassert>

namespace base { namespace warcraft3 { namespace lua_engine {

	void error_function(lua_State* L, bool err_func)
	{
		if (err_func)
		{
			runtime::get_err_function(L);
			if (lua_isfunction(L, -1))
			{
				lua_pushvalue(L, -2);
				safe_call_not_sleep(L, 1, 0, false);
				lua_pop(L, 1);
				return;
			}
			else
			{
				lua_pop(L, 1);
			}
		}

		printf("Error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	int safe_call_not_sleep(lua_State* L, int nargs, int nresults, bool err_func)
	{
		if (err_func)
		{
			runtime::get_err_function(L);
			if (lua_isfunction(L, -1))
			{
				lua_insert(L, 1);
				int error = lua_pcall(L, nargs, nresults, 1);
				lua_remove(L, 1);
				return error;
			}
			else
			{
				lua_pop(L, 1);
			}
		}

		int error = lua_pcall(L, nargs, nresults, 0);
		switch (error)
		{
		case LUA_OK:
			break;
		case LUA_YIELD:
			assert("lua_pcall return LUA_YIELD" || false);
			break;
		default:
			error_function(L, false);
			break;
		}
		return error;
	}

	int safe_resume(lua_State* thread, lua_State* L, int nargs, int& nresults)
	{
		if (!lua_checkstack(thread, nargs + 1)) {
			lua_pushstring(L, "too many arguments to resume");
			nresults = 1;
			return LUA_ERRERR;
		}
		lua_pushvalue(L, -nargs - 1);
		lua_xmove(L, thread, 1);
		lua_xmove(L, thread, nargs);
		int error = lua_resume(thread, L, nargs);

		if (error == LUA_OK || error == LUA_YIELD)
		{
			int nres = lua_gettop(thread);
			if (!lua_checkstack(L, nres + 1)) {
				lua_pop(thread, nres);
				lua_pushstring(L, "too many results to resume");
				nresults = 1;
				return LUA_ERRERR;
			}
			lua_xmove(thread, L, nres);
			nresults = nres;
			return error;
		}
		else
		{
			lua_xmove(thread, L, 1);
			nresults = 1;
			return error;
		}
	}

	int safe_call_has_sleep(lua_State* L, int nargs, int nresults, bool err_func)
	{
		int func_idx = lua_gettop(L) - nargs;
		runtime::thread_create(L, func_idx);
		lua_State* thread = lua_tothread(L, -1);
		int thread_idx = func_idx;
		lua_insert(L, thread_idx);
		func_idx++;

		int n = 0;
		int error = safe_resume(thread, L, nargs, n);
		switch (error)
		{
		case LUA_OK:
		{
			if (nresults > n)
			{
				for (int i = n; i < nresults; i++)
				{
					lua_pushnil(L);
				}
			}
			else if (nresults < n)
			{
				lua_pop(L, n - nresults);
			}
		}
			break;
		case LUA_YIELD:
			runtime::thread_save(L, func_idx, thread_idx);
			break;
		default:
			error_function(thread, err_func);
			break;
		}

		lua_remove(L, func_idx);
		lua_remove(L, thread_idx);
		return error;
	}

	int safe_call(lua_State* L, int nargs, int nresults, bool err_func)
	{
		if (runtime::sleep)
		{
			return safe_call_has_sleep(L, nargs, nresults, err_func);
		}
		else
		{
			return safe_call_not_sleep(L, nargs, nresults, err_func);
		}
	}

	uintptr_t safe_call_ref(lua_State* L, uint32_t ref, size_t nargs, jass::variable_type result_vt)
	{
		runtime::callback_read(L, ref);
		if (!lua_isfunction(L, -1))
		{
			lua_pushstring(L, "safe_call attempt to call (not a function)\n");
			error_function(L, true);
			lua_pop(L, (int)nargs + 1);
			return false;
		}
		lua_insert(L, -(int)nargs-1);

		if (safe_call(L, nargs, (result_vt != jass::TYPE_NOTHING) ? 1 : 0, true) != LUA_OK)
		{
			return 0;
		}
		
		if (result_vt == jass::TYPE_NOTHING)
		{
			return 0;
		}

		uintptr_t ret = jass_read(L, result_vt, -1);
		lua_pop(L, 1);
		return ret;
	}

	uint32_t __fastcall jass_callback(uint32_t ls, uint32_t param)
	{
		LUA_PERFTRACE(kJassEvent);
		return safe_call_ref((lua_State*)ls, param, 0, jass::TYPE_BOOLEAN);
	}

	uint32_t cfunction_to_code(lua_State* L, uint32_t index)
	{
		if (runtime::sleep)
		{
			return jass::trampoline_create(jass_callback, (uintptr_t)get_mainthread(L), runtime::callback_push(L, index));
		}
		else
		{
			return jass::trampoline_create(jass_callback, (uintptr_t)L, runtime::callback_push(L, index));
		}
	}
}}}
