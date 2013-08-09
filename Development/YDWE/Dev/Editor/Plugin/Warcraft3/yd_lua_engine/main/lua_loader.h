#pragma once

#include <cstdint>
#include <ydwe/warcraft3/jass.h>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	class lua_loader
	{
	public:
		static void initialize();

	private:
		static uintptr_t RealCheat;
		static void __cdecl FakeCheat(jass::jstring_t cheat_str);
	};
}}

_BASE_END
