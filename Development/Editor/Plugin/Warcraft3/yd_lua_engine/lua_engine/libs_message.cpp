#include <lua.hpp>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/keyboard_code.h>	
#include <base/warcraft3/player.h>	  
#include <base/warcraft3/message_dispatch.h>
#include <base/hook/inline.h>
#include <base/hook/fp_call.h>
#include "common.h"
#include "callback.h"

namespace base { namespace warcraft3 { namespace lua_engine { namespace message {

	static HWND war3_window = NULL;

	void set_window(HWND hwnd)
	{
		war3_window = hwnd;
	}

	struct AbilityData
	{
		uintptr_t  vft_ptr;
		uintptr_t  ability_id;
		uintptr_t  order_id;
		uintptr_t  unk;
		uintptr_t  target_type;
	};

	struct CCommandButton
	{
		uintptr_t    vft_ptr;
		uintptr_t    unk[0x63];
		AbilityData* ability;   // offset 0x190
	};

	struct message_t
	{
		uint32_t vfptr;
		uint32_t unk01;
		uint32_t msgid;
	};

	struct keyboard_message_t
		: public message_t
	{
		uint32_t unk03;
		uint32_t keycode;
		uint8_t  keystate;
	};

	struct mouse_message_t
		: public message_t
	{
		uint32_t unk03;
		uint32_t unk04;
		float rx;
		float ry;
		float wx;
		float wy;
		uint32_t unk0A;
		uint32_t code;
	};

	struct ability_mouse_message_t
		: public message_t
	{
		uint32_t unk03;
		uint32_t unk04;
		uint32_t unk05;
		uint32_t unk06;
		uint32_t unk07;
		uint32_t unk08;
		uint32_t code;
		uint32_t unk0A;
		uint32_t unk0B;
		CCommandButton* button;
	};

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
		if (s.get_version() >= version_127a)
		{
			ptr += 5;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr += 5;
			ptr = next_opcode(ptr, 0xE8, 5);
		}
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
		POINT pt = { 0 };
		::GetCursorPos(&pt);
		ScreenToClient(war3_window, &pt);
		RECT rt;
		::GetClientRect(war3_window, &rt);
		float fx = 0.8f *((float)(pt.x - rt.left) / (rt.right - rt.left));
		float fy = 0.6f *(1.0f - (float)(pt.y - rt.top) / (rt.bottom - rt.top));
		float wx = 0.f, wy = 0.f;
		convert_xy(fx, fy, wx, wy);
		lua_pushnumber(L, wx);
		lua_pushnumber(L, wy);
		return 2;
	}

	static int lbutton(lua_State* L)
	{
		int x = (int)lua_tointeger(L, 1);
		int y = (int)lua_tointeger(L, 2);
		if (x < 0 || x >= 4 || y < 0 || y >= 3)
		{
			return 0;
		}

		war3_searcher& s = get_war3_searcher();
		uint32_t cgameui = s.get_gameui(0, 0);
		uintptr_t button_bar = (s.get_version() <= version_120e) ? *(uintptr_t*)(cgameui + 0x3BC) : *(uintptr_t*)(cgameui + 0x3C8);
		uintptr_t button_array = *(uintptr_t*)(button_bar + 0x154);
		CCommandButton* button = ((CCommandButton*)*(uintptr_t*)(*(uintptr_t*)(button_array + 0x10 * (y)+0x08) + 0x04 * (x)));
		AbilityData* ability = button->ability;
		if (!ability)
		{
			return 0;
		}
		lua_pushinteger(L, ability->ability_id);
		lua_pushinteger(L, ability->order_id);
		lua_pushinteger(L, ability->target_type & 0x3FF);
		return 3;
	}
	
	static uintptr_t get_select_unit()
	{
		player::selection_t* slt = player::selection(player::local());
		if (!slt || !slt->unit)
		{
			return 0;
		}
		return slt->unit;
	}

	static int lselection(lua_State* L)
	{
		uintptr_t unit = get_select_unit();
		if (!unit)
		{
			jassbind::push_handle(L, 0);
			return 1;
		}
		uintptr_t handle = object_to_handle(unit);
		jassbind::push_handle(L, handle);
		return 1;
	}

	namespace order {
		bool b_hook = false;
		bool b_search = false;

		namespace real {
			uintptr_t immediate_order = 0;
			uintptr_t point_order = 0;
			uintptr_t target_order = 0;
		}

		namespace fake {
			int __fastcall immediate_order(uint32_t order, uint32_t unk, uint32_t flags)
			{
				if (order >= 'A000')
				{
					printf("immediate_order, %c%c%c%c, %X, %X\n", ((char*)&order)[3], ((char*)&order)[2], ((char*)&order)[1], ((char*)&order)[0], unk, flags);
				}
				else
				{
					printf("immediate_order, %X, %X, %X\n", order, unk, flags);
				}
				return fast_call<int>(real::immediate_order, order, unk, flags);
			}

			int __fastcall point_order(uint32_t order, uint32_t unk, float* x, float* y, uint32_t flags)
			{
				if (order >= 'A000')
				{
					printf("point_order, %c%c%c%c, %X, %f, %f, %X\n", ((char*)&order)[3], ((char*)&order)[2], ((char*)&order)[1], ((char*)&order)[0], unk, *x, *y, flags);
				}
				else
				{
					printf("point_order, %X, %X, %f, %f, %X\n", order, unk, *x, *y, flags);
				}
				return fast_call<int>(real::point_order, order, unk, x, y, flags);
			}

			int __fastcall target_order(uint32_t order, uint32_t unk, float* x, float* y, uint32_t target, uint32_t flags)
			{
				if (order >= 'A000')
				{
					printf("target_order, %c%c%c%c, %X, %f, %f, %X, %X\n", ((char*)&order)[3], ((char*)&order)[2], ((char*)&order)[1], ((char*)&order)[0], unk, *x, *y, target, flags);
				}
				else
				{
					printf("target_order, %X, %X, %f, %f, %X, %X\n", order, unk, *x, *y, target, flags);
				}
				return fast_call<int>(real::target_order, order, unk, x, y, target, flags);
			}
		}

		static void search()
		{
			if (b_search) {
				return;
			}
			b_search = true;

			war3_searcher& s = get_war3_searcher();
			uintptr_t ptr = 0;
			if (s.get_version() >= version_127a)
			{
				ptr = s.base() + 0x3AE4E0;
			}
			else
			{
				ptr = s.search_string("SimpleDestructableNameValue");
				ptr += 4;
				do
				{
					ptr = next_opcode(ptr, 0xC1, 3);
					ptr += 3;
				} while (*(uint16_t*)(ptr - 2) != 0x05E2);
			}

			ptr = next_opcode(ptr, 0xE8, 5);
			real::immediate_order = convert_function(ptr);
			ptr += 5;
			ptr = next_opcode(ptr, 0xE8, 5);
			real::point_order = convert_function(ptr);
			ptr += 5;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr += 5;
			ptr = next_opcode(ptr, 0xE8, 5);
			real::target_order = convert_function(ptr);
		}

		static int limmediate(lua_State* L)
		{
			if (!get_select_unit()){
				lua_pushboolean(L, 0);
				return 1;
			}
			search();
			uint32_t order = lua_tointeger(L, 1);
			uint32_t flags = lua_tointeger(L, 2);
			fast_call<int>(real::immediate_order, order, 0, flags);
			lua_pushboolean(L, 1);
			return 1;
		}

		static int lpoint(lua_State* L)
		{
			if (!get_select_unit()){
				lua_pushboolean(L, 0);
				return 1;
			}
			search();
			uint32_t order = lua_tointeger(L, 1);
			float x = (float)lua_tonumber(L, 2);
			float y = (float)lua_tonumber(L, 3);
			uint32_t flags = lua_tointeger(L, 4);
			fast_call<int>(real::point_order, order, 0, &x, &y, flags);
			lua_pushboolean(L, 1);
			return 1;
		}

		static int ltarget(lua_State* L)
		{
			if (!get_select_unit()){
				lua_pushboolean(L, 0);
				return 1;
			}
			search();
			uint32_t order = lua_tointeger(L, 1);
			float x = (float)lua_tonumber(L, 2);
			float y = (float)lua_tonumber(L, 3);
			uint32_t target = jassbind::read_handle(L, 4);
			uint32_t flags = lua_tointeger(L, 5);
			if (target)
			{
				target = handle_to_object(target);
			}
			fast_call<int>(real::target_order, order, 0, &x, &y, target, flags);
			lua_pushboolean(L, 1);
			return 1;
		}

		static int lenable_debug(lua_State* /*L*/)
		{
			if (b_hook) {
				return 0;
			}
			b_hook = true;
			search();

			hook::inline_install(&real::immediate_order, (uintptr_t)fake::immediate_order);
			hook::inline_install(&real::point_order, (uintptr_t)fake::point_order);
			hook::inline_install(&real::target_order, (uintptr_t)fake::target_order);
			return 0;
		}
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
		if (safe_call_not_sleep(L, 1, 1, true) != LUA_OK)
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
		if (safe_call_not_sleep(L, 1, 1, true) != LUA_OK)
		{
			lua_pop(L, 1);
			return true;
		}
		int ok = lua_toboolean(L, -1);
		lua_pop(L, 1);
		lua_pop(L, 1);
		return !!ok;
	}

	static bool ability_mouse_event(lua_State* L, ability_mouse_message_t* msg)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 1);
			return true;
		}
		lua_newtable(L);
		lua_pushstring(L, "mouse_ability");
		lua_setfield(L, -2, "type");
		if (msg && msg->button&& msg->button->ability)
		{
			lua_pushinteger(L, msg->code);
			lua_setfield(L, -2, "code");
			lua_pushinteger(L, msg->button->ability->ability_id);
			lua_setfield(L, -2, "ability");
			lua_pushinteger(L, msg->button->ability->order_id);
			lua_setfield(L, -2, "order");
		}
		lua_insert(L, -2);
		lua_pushvalue(L, -2);
		if (safe_call_not_sleep(L, 1, 1, true) != LUA_OK)
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
		case 0x30064:
			return ability_mouse_event(ML, (ability_mouse_message_t*)msg);
			break;
		case 0x40060064:
			return keyboard_event(ML, "key_down", (keyboard_message_t*)msg);
		case 0x40060066:
			return keyboard_event(ML, "key_up", (keyboard_message_t*)msg);
			break;
		case 0x40060067:
			break;
		case 0x1A0068:
			return mouse_event(ML, "mouse_down", (mouse_message_t*)msg);
		case 0x400500C9:
			return mouse_event(ML, "mouse_up", 0);
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
		static bool hook = false;
		const char* name = lua_tostring(L, 2);
		if (strcmp("hook", name) == 0)
		{
			if (lua_isfunction(L, 3))
			{
				lua_pushvalue(L, 3);
				lua_setfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
				ML = get_mainthread(L);
				if (!hook) {
					message_dispatch::add_filter((uintptr_t)filter);
					hook = !hook;
				}
			}
			else if (lua_isnil(L, 3))
			{
				if (hook) {
					message_dispatch::remove_filter((uintptr_t)filter);
					hook = !hook;
				}
				lua_pushnil(L);
				lua_setfield(L, LUA_REGISTRYINDEX, "_JASS_MESSAGE_HANDLE");
			}
			return 0;
		}
		return 0;
	}

	static int init_keyboard(lua_State* L)
	{
		lua_newtable(L);
#define SET_KEYBOARD(name) \
		lua_pushinteger(L, WARK_ ## name);	\
		lua_setfield(L, -2, #name)
		SET_KEYBOARD_ALL();
#undef SET_KEYBOARD
		return 1;
	}

	int open(lua_State* L)
	{
		lua_newtable(L);
		{
			lua_pushstring(L, "keyboard");
			init_keyboard(L);
			lua_rawset(L, -3);

			luaL_Reg func[] = {
				{ "mouse", lmouse },
				{ "button", lbutton },
				{ "selection", lselection },
				{ NULL, NULL },
			};
			luaL_setfuncs(L, func, 0);

			if (get_war3_searcher().get_version() >= version_124e)
			{
				luaL_Reg func[] = {
					{ "order_immediate", order::limmediate },
					{ "order_point", order::lpoint },
					{ "order_target", order::ltarget },
					{ "order_enable_debug", order::lenable_debug },
					{ NULL, NULL },
				};
				luaL_setfuncs(L, func, 0);
			}

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

		}
		return 1;
	}
}}}}
