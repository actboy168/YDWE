#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <Windows.h>
#include <filesystem>
#include <process.h>
#include <base/util/unicode.h>
#include "LuaEngine/LuaEngine.h"

extern "C" {
	int seri_pack(lua_State* L);
	int seri_unpack(lua_State* L);
}

namespace thread {


	class luathread {
	public:
		luathread(lua_State* L)
			: m_name()
			, m_tid(0)
			, m_param(nullptr)
		{
			size_t len = 0;
			const char* str = luaL_checklstring(L, 1, &len);
			m_name = base::u2w(std::string_view(str, len), base::conv_method::replace | '?');

			lua_replace(L, 1);
			lua_pushcfunction(L, seri_pack);
			lua_insert(L, 2);
			lua_call(L, lua_gettop(L) - 2, 1);
			m_param = lua_touserdata(L, -1);
			lua_pop(L, 1);

			m_tid = _beginthread(&luathread::run, 0, (void*)this);
		}

		static int init(lua_State* L)
		{
			int n = seri_unpack(L);
			lua_createtable(L, n, 0);
			lua_insert(L, 1);
			for (int i = n; i >= 1; i--) {
				lua_seti(L, 1, i);
			}
			lua_setglobal(L, "arg");
			return 0;
		}

		static void run(void* This)
		{
			luathread* self = (luathread*)(This);
			lua_State* L = LuaEngineCreate(self->m_name.c_str());
			if (L) {
				lua_pushcfunction(L, init);
				lua_pushlightuserdata(L, self->m_param);
				if (LUA_OK != lua_pcall(L, 1, 0, 0))
				{
					logging::manager* mgr = logging::get_manager(L);
					LOGGING_ERROR(mgr->get_logger("root")) << "exception: " << lua_tostring(L, -1);
					lua_pop(L, 1);
					LuaEngineDestory(L);
					return;
				}

				LuaEngineStart(L);
				LuaEngineDestory(L);
			}
		}
			
	private:
		std::wstring m_name;
		uintptr_t    m_tid;
		void*        m_param;
	};

	luathread& to(lua_State* L, int idx)
	{
		return *(luathread*)luaL_checkudata(L, idx, "thread");
	}

	int constructor(lua_State* L)
	{
		void* storage = lua_newuserdata(L, sizeof(luathread));
		luaL_getmetatable(L, "thread");
		lua_setmetatable(L, -2);
		new (storage)luathread(L);
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
