
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
#include <base/path/ydwe.h>

namespace NYDWE {

	logging::logger* lg;

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
		if (evenetid >= 0 && evenetid < EVENT_MAXIMUM)
		{
			LOGGING_INFO(lg) << "RegisterEvent: " << lua_tostring(L, 2);
			event_array[evenetid] = std::bind(LuaOnSignal, L, std::placeholders::_1, base::lua::object(L, 3));
			lua_pushboolean(L, 1);
			return 1;
		}
		lua_pushboolean(L, 0);
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

	int32_t CObjectEditor = 0;
	uintptr_t RealCreateCObjectEditor = 0x005935E0;
	int32_t __fastcall FakeCreateCObjectEditor(int32_t This, int32_t Edx, int32_t Unk1, int32_t Unk2)
	{
		CObjectEditor = This;
		return base::fast_call<int32_t>(RealCreateCObjectEditor, This, Edx, Unk1, Unk2);
	}
	
	extern HFONT font;
}

static int set_font(lua_State* L)
{
	if (NYDWE::font) ::DeleteObject(NYDWE::font);
	NYDWE::font = (HFONT)luaL_checkinteger(L, 1);
	return 0;
}

static int import_customdata(lua_State* L)
{
	if (!NYDWE::CObjectEditor) {
		return 0;
	}
	lua_Integer type = luaL_checkinteger(L, 1);
	if (type < 0 || type > 6) {
		return 0;
	}
	int32_t CObjectPane = *(int32_t*)(NYDWE::CObjectEditor + 4 * (8 + type));
	int32_t CCustomData = *(int32_t*)(*(int32_t*)(CObjectPane + 4) + type * 4 + 4);
	fs::path& path = *(fs::path*)luaL_checkudata(L, 2, "filesystem");
	std::string asciipath = base::w2a(path.c_str(), base::conv_method::replace | '?');
	base::fast_call<int32_t>(0x005B7270, CCustomData, 0, asciipath.c_str());
	base::fast_call<int32_t>(0x0064D4E0, CObjectPane);
	return 0;
}

static int ydwe(lua_State* L)
{
	try {
		void* storage = lua_newuserdata(L, sizeof(fs::path));
		luaL_getmetatable(L, "filesystem");
		lua_setmetatable(L, -2);
		new (storage)fs::path(std::move(base::path::ydwe(lua_toboolean(L, 1))));
		return 1;
	}
	catch (const std::exception& e) {
		lua_pushstring(L, base::a2u(e.what()).c_str());
		return lua_error(L);
	}
}

int luaopen_event(lua_State* L)
{
	NYDWE::lg = logging::get_logger(L, "events");
	NYDWE::SetupEvent();

	lua_newtable(L);
	{
#define REGISTER_EID(name) \
	lua_pushstring(L, #name); \
	lua_pushinteger(L, NYDWE:: ## name); \
	lua_rawset(L, -3); \
    NYDWE::event_array[NYDWE:: ## name] = [](NYDWE::TEventData eventData)->int{ return -1; };

		REGISTER_EID(EVENT_WE_START);
		REGISTER_EID(EVENT_WE_EXIT);
		REGISTER_EID(EVENT_TEST_MAP);
		REGISTER_EID(EVENT_INIT_MENU);
		REGISTER_EID(EVENT_MSS_LOAD);
		REGISTER_EID(EVENT_WINDOW_MESSAGE);
		REGISTER_EID(EVENT_DIALOG_MESSAGE);
		REGISTER_EID(EVENT_NEW_OBJECT_ID);
		REGISTER_EID(EVENT_NEW_SAVE_MAP);
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

	base::hook::install(&NYDWE::RealWeMessageShow, (uintptr_t)NYDWE::FakeWeMessageShow);
	base::hook::install(&NYDWE::RealCreateCObjectEditor, (uintptr_t)NYDWE::FakeCreateCObjectEditor);

	luaL_Reg lib[] = {
		{ "message_show", NYDWE::LuaWeMessageShow },
		{ "set_font", set_font },
		{ "import_customdata", import_customdata },
		{ "ydwe", ydwe },
		{ NULL, NULL },
	};
	luaL_newlib(L, lib);
	lua_setglobal(L, "we");
	return 0;
}
