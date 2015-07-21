#pragma once

#include <lua.hpp>

namespace base { namespace warcraft3 { namespace lua_engine { namespace runtime {
		extern int handle_level;
		extern bool sleep;
		extern bool catch_crash;
		extern bool enable_console;

		void initialize();

		int get_err_function(lua_State* L);
		int get_global_table(lua_State* L, const char* name, bool weak);

		int thread_create(lua_State* L, int index);
		int thread_save(lua_State* L, int key, int value);

		int handle_ud_get_table(lua_State* L);

		int callback_push(lua_State* L, int idx);
		int callback_read(lua_State* L, int ref);
}}}}
