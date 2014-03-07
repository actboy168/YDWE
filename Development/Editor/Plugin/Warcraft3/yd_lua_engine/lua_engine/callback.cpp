#include "callback.h"
#include "jassbind.h"
#include "libs_runtime.h"
#include "common.h"
#include <base/warcraft3/jass/trampoline_function.h>
#include <Windows.h>
#include <cassert>

namespace base { namespace warcraft3 { namespace lua_engine {

	lua::state* get_mainthread(lua::state* thread)
	{
		lua_rawgeti(thread->self(), LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		lua::state* ml = (lua::state*)thread->tothread(-1);
		thread->pop(1);
		return ml;
	}

	void error_function(lua::state* ls, bool err_func)
	{
		if (err_func)
		{
			runtime::get_err_function(ls);
			if (ls->isfunction(-1))
			{
				ls->pushvalue(-2);
				safe_call(ls, 1, 0, false);
				ls->pop(2);
				return;
			}
			else
			{
				ls->pop(1);
			}
		}

		printf("Error: %s\n", ls->tostring(-1));
		ls->pop(1);
	}

	int safe_call_not_sleep(lua::state* ls, int nargs, int nresults, bool err_func)
	{
		if (err_func)
		{
			runtime::get_err_function(ls);
			if (ls->isfunction(-1))
			{
				ls->insert(1);
				int error = lua_pcall(ls->self(), nargs, nresults, 1);
				ls->remove(1);
				return error;
			}
			else
			{
				ls->pop(1);
			}
		}

		int error = lua_pcall(ls->self(), nargs, nresults, 0);
		switch (error)
		{
		case LUA_OK:
			break;
		case LUA_YIELD:
			assert("lua_pcall return LUA_YIELD" || false);
			break;
		default:
			error_function(ls, false);
			break;
		}
		return error;
	}

	int safe_resume(lua::state* thread, lua::state* ls, int nargs, int& nresults)
	{
		if (!thread->checkstack(nargs + 1)) {
			ls->pushstring("too many arguments to resume");
			nresults = 1;
			return LUA_ERRERR;
		}
		ls->pushvalue(-nargs-1);
		ls->xmove(thread, 1);
		ls->xmove(thread, nargs);
		int error = lua_resume(thread->self(), ls->self(), nargs);

		if (error == LUA_OK || error == LUA_YIELD)
		{
			int nres = thread->gettop();
			if (!ls->checkstack(nres + 1)) {
				thread->pop(nres);
				ls->pushstring("too many results to resume");
				nresults = 1;
				return LUA_ERRERR;
			}
			thread->xmove(ls, nres);
			nresults = nres;
			return error;
		}
		else
		{
			thread->xmove(ls, 1);
			nresults = 1;
			return error;
		}
	}

	int safe_call_has_sleep(lua::state* ls, int nargs, int /*nresults*/, bool err_func)
	{
		int func_idx = ls->gettop() - nargs;
		runtime::thread_create(ls, func_idx);
		lua::state* thread = ls->tothread(-1);
		int thread_idx = ls->gettop() - (nargs + 1);
		ls->insert(thread_idx);
		func_idx++;

		int nresults = 0;
		int error = safe_resume(thread, ls, nargs, nresults);

		switch (error)
		{
		case LUA_OK:
			break;
		case LUA_YIELD:
			runtime::thread_save(ls, func_idx, thread_idx);
			break;
		default:
			error_function(ls, err_func);
			break;
		}

		ls->remove(thread_idx);
		return error;
	}

	int safe_call(lua::state* ls, int nargs, int nresults, bool err_func)
	{
		if (runtime::sleep)
		{
			return safe_call_has_sleep(ls, nargs, nresults, err_func);
		}
		else
		{
			return safe_call_not_sleep(ls, nargs, nresults, err_func);
		}
	}

	uintptr_t safe_call_ref(lua::state* ls, uint32_t ref, size_t nargs, jass::variable_type result_vt)
	{
		int base = ls->gettop() - nargs + 1;
		runtime::callback_read(ls, ref);
		if (!ls->isfunction(-1))
		{
			ls->pushstring("safe_call attempt to call (not a function)\n");
			error_function(ls, true);
			ls->pop(1);
			return false;
		}
		ls->insert(base);

		if (safe_call(ls, nargs, (result_vt != jass::TYPE_NOTHING) ? 1 : 0, true) != LUA_OK)
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

	uint32_t __fastcall jass_callback(uint32_t ls, uint32_t param)
	{
		return safe_call_ref((lua::state*)ls, param, 0, jass::TYPE_BOOLEAN);
	}

	uint32_t cfunction_to_code(lua::state* ls, uint32_t index)
	{
		if (runtime::sleep)
		{
			return jass::trampoline_create(jass_callback, (uintptr_t)get_mainthread(ls), runtime::callback_push(ls, index));
		}
		else
		{
			return jass::trampoline_create(jass_callback, (uintptr_t)ls->self(), runtime::callback_push(ls, index));
		}
	}
}}}
