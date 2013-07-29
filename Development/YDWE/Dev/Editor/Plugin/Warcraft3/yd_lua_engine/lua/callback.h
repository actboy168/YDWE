#pragma once

#include <ydwe/lua/state.h>
#include <lua.hpp>
#include <cstdint>

namespace ydwe { namespace warcraft3 { namespace lua_engine { namespace lua {

	using namespace ydwe::lua;

	state*& instance();	
	void    do_buffer(const char* name, const char* buffer, size_t size);

	struct callback
	{
	public:
		callback();
		callback(lua::state* ls, uint32_t index);
		callback(uint32_t ref);
		operator uint32_t();
		int      create(lua::state* ls, uint32_t index);
		uint32_t call() const;

	private:
		uint32_t ref_;
	};


}}}}
