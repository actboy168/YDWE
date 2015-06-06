
#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <boost/foreach.hpp>
#include "YDWEEvent.h"
#include "YDWELogger.h"
#include <base/hook/fp_call.h>
#include <base/exception/exception.h>

namespace NYDWE {

	struct CEventDataTranslationVisitor : public boost::static_visitor<>
	{
		CEventDataTranslationVisitor(lua_State *pState, const std::string &key)
			: pState_(pState), key_(key) {}

		template <typename T>
		void operator()(T &e) const
		{
			lua_pushlstring(pState_, key_.data(), key_.size());
			luabind::object(pState_, e).push(pState_);
			lua_settable(pState_, -3);
		}

		lua_State *pState_;
		const std::string &key_;
	};

	static void TranslateEventDataToLuaTable(lua_State *pState, CYDWEEventData &eventData)
	{
		lua_newtable(pState);
		BOOST_FOREACH(auto e, eventData.getDataStore())
		{
			boost::apply_visitor(CEventDataTranslationVisitor(pState, e.first), e.second);
		}
	}

	struct lua_stack_guard
	{
		lua_stack_guard(lua_State* pState)
			: state_(pState)
			, idx_(lua_gettop(state_))
		{ }

		~lua_stack_guard()
		{
			lua_settop(state_, idx_);
		}

		lua_State* state_;
		int        idx_;
	};

	static int LuaOnSignal(lua_State *pState, CYDWEEventData &eventData, bool ignore_error, luabind::object const& func)
	{
		lua_stack_guard tmp_guard(pState);

		TranslateEventDataToLuaTable(pState, eventData);
		luabind::object event_data(luabind::from_stack(pState, -1));

		if (ignore_error) 
		{
			try {
				return luabind::call_function<int>(func, event_data);
			}
			catch (...) {
			}
		}
		else
		{
			try {
				return luabind::call_function<int>(func, event_data);
			}
			catch (luabind::error const& e) {
				LOGGING_ERROR(lg) << "exception: \"" << e.what() << "\" " << lua_tostring(e.state(), -1);
			}
			catch (std::exception const& e) {
				LOGGING_ERROR(lg) << "exception: \"" << e.what() << "\"";
			}
			catch (...) {
				LOGGING_ERROR(lg) << "unknown exception";
			}
		}

		return -1;
	}

	void LuaRegisterEvent(lua_State *pState, EVENT_ID evenetid, bool ignore_error, luabind::object const& func)
	{
		LOGGING_TRACE(lg) << "RegisterEvent id: " << evenetid << " ignore: " << (ignore_error ? "true" : "false");

		if (evenetid >= 0 && evenetid < EVENT_MAXIMUM)
		{
			event_array[evenetid].connect       (std::bind(LuaOnSignal, pState, std::placeholders::_1, ignore_error, func));
		}
	}

	int event_index(lua_State* L)
	{
		lua_pushnil(L);
		return 1;
	}

	int event_newindex(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);
		lua_pushstring(L, "eid");
		lua_rawget(L, 1);
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1))
		{
			EVENT_ID evenetid = (EVENT_ID)lua_tointeger(L, -1);
			LuaRegisterEvent(L, evenetid, false, luabind::object(luabind::from_stack(L, 3)));
			lua_pushboolean(L, 1);
		}
		else
		{
			lua_pushboolean(L, 0);
		}
		return 1;
	}


	void WeMessageShow(const char *message)
	{
		base::fast_call<void>(0x004D5900, message, 0);
	}

	void SetupEvent();
}

int luaopen_event(lua_State* L)
{
	NYDWE::SetupEvent();

	lua_newtable(L);
	{
		lua_pushstring(L, "eid");
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
		lua_rawset(L, -3);

		lua_newtable(L);
		{
			lua_pushstring(L, "__index");
			lua_pushcclosure(L, NYDWE::event_index, 0);
			lua_rawset(L, -3);
			lua_pushstring(L, "__newindex");
			lua_pushcclosure(L, NYDWE::event_newindex, 0);
			lua_rawset(L, -3);
		}
		lua_setmetatable(L, -2);
	}
	lua_setglobal(L, "event");

	using namespace luabind;
	module(L, "we")
	[
		def("message_show", &NYDWE::WeMessageShow)
	];
	return 0;
}
