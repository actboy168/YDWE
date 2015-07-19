#include <lua.hpp>
#include <base/warcraft3/war3_searcher.h>		  
#include <base/warcraft3/version.h>			  
#include <base/hook/inline.h>				  
#include <base/hook/fp_call.h>	  
#include "common.h"	  		  
#include "callback.h"

namespace base { namespace warcraft3 { namespace lua_engine {

	struct message_t
	{
		uint32_t vfptr;
		uint32_t unk01;
		uint32_t msgid;
	};

	struct keyboard_message_t
		: public message_t
	{
		uint32_t unk04;
		uint32_t keycode;
		uint8_t  keystate;
	};

	struct mouse_message_t
		: public message_t
	{
		uint32_t unk04;
		uint32_t unk05;
		float rx;
		float ry;
		float wx;
		float wy;
		uint32_t unk0A;
		uint32_t code;
	};

	namespace message_dispatch
	{
		static uintptr_t search()
		{
			war3_searcher& s = get_war3_searcher();
			if (s.get_version() == version_120e)
			{
				return s.base() + 0x0070CD34;
			}
			uintptr_t ptr = s.search_string_ptr("QuickSave", sizeof("QuickSave"));
			ptr += 0x44;
			hook::replace_pointer(0, 0);
			return ptr;
		}

		static uintptr_t get()
		{
			static uintptr_t ptr = search();
			return ptr;
		}

		static uintptr_t fn_real = 0;
		static uintptr_t fn_filter = 0;
		static bool __fastcall fn_fake(uintptr_t cgameui, uintptr_t /*edx*/, message_t* msg)
		{
			if (!this_call<bool>(fn_filter, cgameui, msg))
			{
				return false;
			}
			return this_call<bool>(fn_real, cgameui, msg);
		}

		static void hook(uintptr_t filter)
		{
			fn_filter = filter;
			fn_real = hook::replace_pointer(get(), (uintptr_t)fn_fake);
		}

		static void unhook()
		{
			hook::replace_pointer(get(), (uintptr_t)fn_real);
			fn_filter = 0;
		}
	}

	static uintptr_t search_convert_xy()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = 0;
		if (s.get_version() <= version_120e)
		{
			ptr = s.search_string("E:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI\\CWorldFrameWar3.cpp");
			ptr += 4;
			ptr = next_opcode(ptr, 0xC7, 6);
			ptr += 6;
			ptr = next_opcode(ptr, 0xC7, 6);
			ptr += 6;
			ptr = next_opcode(ptr, 0xC7, 6);
			ptr += 2;
			ptr = *(uintptr_t*)ptr;
		}
		else
		{
			ptr = get_vfn_ptr(".?AVCWorldFrameWar3@@");
		}
		ptr += 0x38;
		ptr = *(uintptr_t*)ptr;
		do
		{
			ptr = next_opcode(ptr, 0x68, 5);
			ptr += 5;
		} while (*(uintptr_t*)(ptr - 4) != 0x1A0068);
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr += 5;
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);
		return ptr;
	}

	static int convert_xy(float rx, float ry, float& wx, float& wy)
	{
		static uintptr_t s_convert_xy = search_convert_xy();
		war3_searcher& s = get_war3_searcher();
		uint32_t cgameui = s.get_gameui(0, 0);
		float buf[0x10] = { 0 };
		uintptr_t cworldframe = (s.get_version() <= version_120e) ? *(uintptr_t*)(cgameui + 0x3B0) : *(uintptr_t*)(cgameui + 0x3BC);
		int result = this_call<int>(s_convert_xy, cworldframe, rx, ry, buf, 1, 1);
		wx = buf[4];
		wy = buf[5];
		return result;
	}

	static int lmouse(lua_State* L)
	{
		HWND hwnd = ::FindWindowW(L"Warcraft III", L"Warcraft III");
		POINT pt = { 0 };
		::GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);
		RECT rt;
		::GetClientRect(hwnd, &rt);
		float fx = 0.8 *((float)(pt.x - rt.left) / (rt.right - rt.left));
		float fy = 0.6 *(1.0 - (float)(pt.y - rt.top) / (rt.bottom - rt.top));
		float wx = 0, wy = 0;
		convert_xy(fx, fy, wx, wy);
		lua_pushnumber(L, wx);
		lua_pushnumber(L, wy);
		return 2;
	}

	static lua_State* ML = 0;
	static bool keyboard_event(lua_State* L, const char* type, keyboard_message_t* msg)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 1);
			return true;
		}
		lua_newtable(L);
		lua_pushstring(L, type);
		lua_setfield(L, -2, "type");
		if (msg)
		{
			lua_pushinteger(L, msg->keycode);
			lua_setfield(L, -2, "code");
			lua_pushinteger(L, msg->keystate);
			lua_setfield(L, -2, "state");
		}
		lua_insert(L, -2);
		lua_pushvalue(L, -2);
		if (safe_call(L, 1, 1, true) != LUA_OK)
		{
			lua_pop(L, 1);
			return true;
		}
		int ok = lua_toboolean(L, -1);
		lua_pop(L, 1);
		if (ok && msg)
		{
			lua_getfield(L, -1, "code");
			msg->keycode = lua_tointeger(L, -1);
			lua_getfield(L, -2, "state");
			msg->keystate = lua_tointeger(L, -1);
			lua_pop(L, 2);
		}
		lua_pop(L, 1);
		return !!ok;
	}

	static bool mouse_event(lua_State* L, const char* type, mouse_message_t* msg)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 1);
			return true;
		}
		lua_newtable(L);
		lua_pushstring(L, type);
		lua_setfield(L, -2, "type");
		if (msg)
		{
			lua_pushinteger(L, msg->code);
			lua_setfield(L, -2, "code");
			lua_pushnumber(L, msg->wx);
			lua_setfield(L, -2, "x");
			lua_pushnumber(L, msg->wy);
			lua_setfield(L, -2, "y");
		}
		lua_insert(L, -2);
		lua_pushvalue(L, -2);
		if (safe_call(L, 1, 1, true) != LUA_OK)
		{
			lua_pop(L, 1);
			return true;
		}
		int ok = lua_toboolean(L, -1);
		lua_pop(L, 1);
		lua_pop(L, 1);
		return !!ok;
	}

	static bool __fastcall filter(uintptr_t /*cgameui*/, uintptr_t /*edx*/, message_t* msg)
	{
		switch (msg->msgid)
		{
		case 0x40060064:
			keyboard_event(ML, "key_down", (keyboard_message_t*)msg);
			break;
		case 0x40060066:
			keyboard_event(ML, "key_up", (keyboard_message_t*)msg);
			break;
		case 0x40060067:
			break;
		case 0x1A0068:
			mouse_event(ML, "mouse_down", (mouse_message_t*)msg);
			break;
		case 0x400500C9:
			mouse_event(ML, "mouse_up", 0);
			break;
		default:
			break;
		}
		return true;
	}

	static int lget(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);
		if (strcmp("hook", name) == 0)
		{
			lua_getfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
			return 1;
		}
		return 0;
	}

	static int lset(lua_State* L)
	{
		const char* name = lua_tostring(L, 2);
		if (strcmp("hook", name) == 0)
		{
			if (lua_isfunction(L, 3))
			{
				lua_pushvalue(L, 3);
				lua_setfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
				ML = get_mainthread(L);
				message_dispatch::hook((uintptr_t)filter);
			}
			else if (lua_isnil(L, 3))
			{
				message_dispatch::unhook();
				lua_pushnil(L);
				lua_setfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
			}
			return 0;
		}
		return 0;
	}

	int jass_message(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, lget, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, lset, 0);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);

			lua_pushstring(L, "mouse");
			lua_pushcclosure(L, lmouse, 0);
			lua_rawset(L, -3);
		}
		return 1;
	}
}}}
