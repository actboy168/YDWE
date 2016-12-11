#pragma once

#include <lua.hpp>	
#include <stdint.h>	

#define LUA_PERFTRACE(ID) \
	::base::warcraft3::lua_engine::runtime::perftrace::guard guard_ ## __LINE__( ::base::warcraft3::lua_engine::runtime::perftrace:: ## ID)

namespace base { namespace warcraft3 { namespace lua_engine { namespace runtime {
		extern int handle_level;
		extern bool sleep;
		extern bool catch_crash;
		extern bool enable_console;

		namespace perftrace {
			static const size_t kJassCall = 0;
			static const size_t kJassEvent = 1;

			struct guard
			{
				guard(size_t id);
				~guard();
				int id_;
				uint64_t start_;
			};
		}

		void initialize();

		int get_err_function(lua_State* L);
		int get_global_table(lua_State* L, const char* name, bool weak);

		int thread_create(lua_State* L, int index);
		int thread_save(lua_State* L, int key, int value);

		int handle_ud_get_table(lua_State* L);

		int callback_push(lua_State* L, int idx);
		int callback_read(lua_State* L, int ref);
}}}}
