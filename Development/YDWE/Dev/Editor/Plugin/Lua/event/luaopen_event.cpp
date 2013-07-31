
#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <ydwe/lua/luabind.h>
#pragma warning(pop)
#include <boost/foreach.hpp>
#include "YDWEEvent.h"
#include "YDWELogger.h"
#include <aero/function/fp_call.hpp>
#include <ydwe/exception/exception.h>

namespace NYDWE {

	struct CEventDataTranslationVisitor : public boost::static_visitor<>
	{
		CEventDataTranslationVisitor(lua_State *pState, const std::string &key)
			: pState_(pState), key_(key) {}

		template <typename T>
		void operator()(T &e) const
		{
			lua_pushlstring(pState_, key_.data(), key_.size());
			lua_pushnumber(pState_, e);
			lua_settable(pState_, -3);
		}

		template <>
		void operator()<bool>(bool &e) const
		{
			lua_pushlstring(pState_, key_.data(), key_.size());
			lua_pushboolean(pState_, e);
			lua_settable(pState_, -3);
		}

		template <>
		void operator()<std::string>(std::string &e) const
		{
			lua_pushlstring(pState_, key_.data(), key_.size());
			lua_pushlstring(pState_, e.data(), e.size());
			lua_settable(pState_, -3);
		}

		template <>
		void operator()<void *>(void *&e) const
		{
			lua_pushlstring(pState_, key_.data(), key_.size());
			if (e)
				lua_pushlightuserdata(pState_, e);
			else
				lua_pushnil(pState_);

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
				LOG4CXX_ERROR(NYDWE::gInjectLogger, "exception: \"" << e.what() << "\" " << lua_tostring(e.state(), -1));
			}
			catch (ydwe::exception const& e) {
				LOG4CXX_ERROR(NYDWE::gInjectLogger, L"exception: \"" << e.c_str() << L"\"");
			}
			catch (std::exception const& e) {
				LOG4CXX_ERROR(NYDWE::gInjectLogger, "exception: \"" << e.what() << "\"");
			}
			catch (...) {
				LOG4CXX_ERROR(NYDWE::gInjectLogger, "unknown exception");
			}
		}

		return -1;
	}

	void LuaRegisterEvent(lua_State *pState, EVENT_ID evenetid, bool ignore_error, luabind::object const& func)
	{
		LOG4CXX_TRACE(NYDWE::gInjectLogger, "RegisterEvent id: " << evenetid << " ignore_error: " << (ignore_error ? "true" : "false"));

		if (evenetid >= 0 && evenetid < EVENT_MAXIMUM)
		{
			event_array[evenetid].connect       (std::bind(LuaOnSignal, pState, std::placeholders::_1, ignore_error, func));
		}
	}

	void WeMessageShow(const char *message, int flag)
	{
		aero::fast_call<void>(0x004D5900, message, flag);
	}

	void SetupEvent();
}

int luaopen_event(lua_State *pState)
{
	NYDWE::SetupEvent();

	using namespace luabind;

	module(pState, "event")
	[
		def("register", &NYDWE::LuaRegisterEvent)		
	];

	module(pState, "we")
	[
		def("message_show", &NYDWE::WeMessageShow)
	];

	lua_getglobal(pState, "event");
	object constantTable(from_stack(pState, -1));

#define LUA_AREA_CONSTANT(name) constantTable[#name] = NYDWE:: ## name
	LUA_AREA_CONSTANT(EVENT_WE_START);
	LUA_AREA_CONSTANT(EVENT_WE_EXIT);
	LUA_AREA_CONSTANT(EVENT_SAVE_MAP);
	LUA_AREA_CONSTANT(EVENT_TEST_MAP);
	LUA_AREA_CONSTANT(EVENT_INIT_MENU);
	LUA_AREA_CONSTANT(EVENT_MSS_LOAD);
	LUA_AREA_CONSTANT(EVENT_WINDOW_MESSAGE);
	LUA_AREA_CONSTANT(EVENT_DIALOG_MESSAGE);	
	LUA_AREA_CONSTANT(EVENT_NEW_OBJECT_ID);

	return 0;
}
