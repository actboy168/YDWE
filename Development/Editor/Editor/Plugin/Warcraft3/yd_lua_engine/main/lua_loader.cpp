#include "../lua/callback.h"
#include "../lua/helper.h"
#include "../main/lua_loader.h"
#include "../misc/storm.h"
#include <ydwe/lua/state.h>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/util/singleton.h>
#include <aero/function/fp_call.hpp>

namespace ydwe { namespace warcraft3 { namespace lua_engine {

	uintptr_t lua_loader::RealCheat = 0;

	void lua_loader::initialize()
	{
		native_function::async_hook("Cheat", (uintptr_t*)&RealCheat, (uintptr_t)FakeCheat);
	}

	int open_libs(lua::state* ls)
	{
		luaL_requiref(ls->self(), "_G",            luaopen_base, 1);      ls->pop(1);
		luaL_requiref(ls->self(), LUA_LOADLIBNAME, luaopen_package, 1);   ls->pop(1);
		luaL_requiref(ls->self(), LUA_COLIBNAME,   luaopen_coroutine, 1); ls->pop(1);
		luaL_requiref(ls->self(), LUA_TABLIBNAME,  luaopen_table, 1);     ls->pop(1);
		luaL_requiref(ls->self(), LUA_STRLIBNAME,  luaopen_string, 1);    ls->pop(1);
		luaL_requiref(ls->self(), LUA_BITLIBNAME,  luaopen_bit32, 1);     ls->pop(1);
		luaL_requiref(ls->self(), LUA_MATHLIBNAME, luaopen_math, 1);      ls->pop(1);

		return 1;
	}

	int open_jass(lua::state* ls);
	int open_japi(lua::state* ls);
	int open_jass_ext(lua::state* ls);

	int open_lua_engine(lua::state* ls)
	{
		if (!lua::instance())
		{
			lua::instance() = ls;
			open_jass(ls);
			open_japi(ls);
			open_jass_ext(ls);
			lua::insert_searchers_table(ls);
		}
		return 1;
	}

	void __cdecl lua_loader::FakeCheat(jass::jstring_t cheat_str)
	{
		const char* cheat = jass::from_string(cheat_str);

		if (!cheat)
		{
			aero::c_call<uint32_t>(RealCheat, cheat_str);
			return ;
		}

		std::string cheat_s = cheat;
		if (cheat_s.compare(0, 4, "run ") == 0)
		{
			lua::state*& ls = lua::instance();
			if (!ls)
			{
				ls = (lua::state*)luaL_newstate();
				open_libs(ls);
				open_jass(ls);
				open_japi(ls);
				open_jass_ext(ls);
				lua::replace_searchers_table(ls);
			}

			cheat_s = cheat_s.substr(4);

			const char* buffer = nullptr;
			size_t      size   = 0;
			storm& s = util::singleton_nonthreadsafe<storm>::instance();
			if (s.load_file(cheat_s.c_str(), (const void**)&buffer, &size))
			{
				lua::do_buffer(cheat_s.c_str(), buffer, size);
				s.unload_file(buffer);
			}
		}

		aero::c_call<uint32_t>(RealCheat, cheat_str);
	}
}}}
