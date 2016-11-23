
#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include "YDWEEvent.h"
#include "YDWELogger.h"
#include <base/hook/fp_call.h>	
#include <base/hook/inline.h>
#include <base/exception/exception.h>
#include <base/lua/object.h>
#include <base/lua/guard.h>

namespace NYDWE {

	static int LuaOnSignal(lua_State* L, TEventData eventData, const base::lua::object& func)
	{
		base::lua::guard guard(L);

		try {
			func.push();
			lua_newtable(L);
			eventData(L, lua_absindex(L, -1));
			if (LUA_OK != lua_pcall(L, 1, 1, 0)) {
				throw std::exception(lua_tostring(L, -1));
			}
			return (int)lua_tointeger(L, -1);
		}
		catch (std::exception const& e) {
			LOGGING_ERROR(lg) << "exception: \"" << e.what() << "\"";
		}
		catch (...) {
			LOGGING_ERROR(lg) << "unknown exception";
		}
		return -1;
	}

	int event_index(lua_State* L)
	{
		lua_pushnil(L);
		return 1;
	}

	int event_newindex(lua_State* L)
	{
		lua_pushvalue(L, 2);
		if (LUA_TNIL == lua_rawget(L, lua_upvalueindex(1))) {
			lua_pushboolean(L, 0);
			return 1;
		}

		EVENT_ID evenetid = (EVENT_ID)lua_tointeger(L, -1);
		LOGGING_TRACE(lg) << "RegisterEvent id: " << evenetid;
		if (evenetid >= 0 && evenetid < EVENT_MAXIMUM)
		{
			event_array[evenetid] = std::bind(LuaOnSignal, L, std::placeholders::_1, base::lua::object(L, 3));
		}
		lua_pushboolean(L, 1);
		return 1;
	}

	uintptr_t RealWeMessageShow = 0x004D5900;
	int LuaWeMessageShow(lua_State* L)
	{
		const char* message = lua_tostring(L, 1);
		base::fast_call<void>(RealWeMessageShow, message, 0);
		return 0;
	}

	void __fastcall FakeWeMessageShow(const char *message, int flag)
	{
		LOGGING_INFO(lg) << message;
		base::fast_call<void>(RealWeMessageShow, message, flag);
	}
}

int luaopen_event(lua_State* L)
{
	NYDWE::SetupEvent();

	lua_newtable(L);
	{
#define REGISTER_EID(name) \
	lua_pushstring(L, #name); \
	lua_pushinteger(L, NYDWE:: ## name); \
	lua_rawset(L, -3);	
		REGISTER_EID(EVENT_WE_START);
		REGISTER_EID(EVENT_WE_EXIT);
		REGISTER_EID(EVENT_PRE_SAVE_MAP);
		REGISTER_EID(EVENT_SAVE_MAP);
		REGISTER_EID(EVENT_TEST_MAP);
		REGISTER_EID(EVENT_INIT_MENU);
		REGISTER_EID(EVENT_MSS_LOAD);
		REGISTER_EID(EVENT_WINDOW_MESSAGE);
		REGISTER_EID(EVENT_DIALOG_MESSAGE);
		REGISTER_EID(EVENT_NEW_OBJECT_ID);
#undef REGISTER_EID
	}

	lua_newtable(L);
	{
		lua_newtable(L);
		{
			lua_pushstring(L, "__index");
			lua_pushvalue(L, -4);
			lua_pushcclosure(L, NYDWE::event_index, 1);
			lua_rawset(L, -3);
			lua_pushstring(L, "__newindex");
			lua_pushvalue(L, -4);
			lua_pushcclosure(L, NYDWE::event_newindex, 1);
			lua_rawset(L, -3);
		}
		lua_setmetatable(L, -2);
	}
	lua_setglobal(L, "event");

	base::hook::inline_install(&NYDWE::RealWeMessageShow, (uintptr_t)NYDWE::FakeWeMessageShow);

	lua_newtable(L);
	{
		lua_pushstring(L, "message_show");
		lua_pushcclosure(L, NYDWE::LuaWeMessageShow, 0);
		lua_rawset(L, -3);
	}
	lua_setglobal(L, "we");
	return 0;
}
