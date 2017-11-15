#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <Windows.h>
#include <filesystem>
#include <process.h>
#include <base/util/unicode.h>
#include "LuaEngine/LuaEngine.h"

namespace thread {

	class luathread {
	public:
		luathread(const std::wstring& name)
			: m_name(name)
			, m_tid(_beginthread(&luathread::run, 0, (void*)this))
		{ }

		static void run(void* This)
		{
			luathread* self = (luathread*)(This);
			lua_State* L = LuaEngineCreate(self->m_name.c_str());
			if (L) {
				LuaEngineDestory(L);
			}
		}

	private:
		std::wstring m_name;
		uintptr_t    m_tid;
	};

	std::wstring to_stdwstring(lua_State* L, int idx)
	{
		size_t len = 0;
		const char* str = luaL_checklstring(L, idx, &len);
		return base::u2w(std::string_view(str, len), base::conv_method::replace | '?');
	}

	luathread& to(lua_State* L, int idx)
	{
		return *(luathread*)luaL_checkudata(L, idx, "thread");
	}

	int constructor(lua_State* L)
	{
		void* storage = lua_newuserdata(L, sizeof(luathread));
		luaL_getmetatable(L, "thread");
		lua_setmetatable(L, -2);
		new (storage)luathread(to_stdwstring(L, 1));
		return 1;
	}

	static int destructor(lua_State* L)
	{
		luathread& self = to(L, 1);
		self.~luathread();
		return 0;
	}
}

extern "C" __declspec(dllexport)
int luaopen_thread(lua_State* L)
{
	static luaL_Reg mt[] = {
		{ "__gc", thread::destructor },
		{ NULL, NULL }
	};
	luaL_newmetatable(L, "thread");
	luaL_setfuncs(L, mt, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, thread::constructor);
	return 1;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
