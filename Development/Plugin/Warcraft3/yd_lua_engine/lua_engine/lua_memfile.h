#pragma once

#include <lua.hpp>

namespace memfile {
	typedef int(*IFileCreate)(const char* filename, unsigned char** data, size_t* size);
	typedef int(*IFileClose)(unsigned char* data, size_t size);
	void initialize(lua_State *L);
	int open(lua_State *L, IFileCreate icreate, IFileClose iclose);
}
