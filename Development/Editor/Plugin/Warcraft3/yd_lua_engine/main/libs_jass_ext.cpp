#include "../lua/helper.h"
#include "../lua/jassbind.h"
#include "../lua/callback.h"
#include <base/util/console.h>
#include <base/util/do_once.h>
#include <base/warcraft3/jass/func_value.h>
#include <aero/function/fp_call.hpp>
#include <base/hook/inline.h>

namespace base { namespace warcraft3 { namespace lua_engine {


	int  install_jass_hook(jassbind*, const jass::func_value* nf, const char* name, const callback& fake_func);
	int  uninstall_jass_hook(jassbind*, const char* name);

	int jass_hook_set(lua_State* L)
	{
		jassbind* lj = (jassbind*)L;
		const char* name = lj->tostring(2);

		const jass::func_value* nf = jass::jass_func(name);
		if (nf && nf->is_valid())
		{
			if (lj->isfunction(3))
			{
				install_jass_hook(lj, nf, name, callback(lj, 3));
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

	int jass_enable_console(lua_State* /*L*/)
	{
		util::console::enable();
		util::console::disable_close_button();

		return 0;
	}

	int open_jass_ext(lua::state* ls)
	{
		ls->newtable();
		{
			ls->pushstring("hook");
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
			ls->rawset(-3);

			ls->pushstring("EnableConsole");
			ls->pushcclosure((lua::state::cfunction)jass_enable_console, 0);
			ls->rawset(-3);
		}
		ls->setglobal("jass_ext");
		return 1;
	}
}}}
