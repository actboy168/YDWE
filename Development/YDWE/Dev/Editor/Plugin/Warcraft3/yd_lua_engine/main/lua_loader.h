#pragma once

#include <cstdint>
#include <ydwe/warcraft3/jass.h>


namespace ydwe { namespace warcraft3 { namespace lua_engine {

	class lua_loader
	{
	public:
		static void initialize();

	private:
		static uintptr_t RealCheat;
		static void __cdecl FakeCheat(jass::jstring_t cheat_str);
	};
}}}
