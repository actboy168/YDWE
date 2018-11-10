#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <Windows.h>
#include <queue>
#include <mutex>
#include <process.h>
#include <bee/utility/unicode.h>
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
			m_name = bee::u2w(std::string_view(str, len));

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
					LOGGING_ERROR(mgr->get_logger("launch")) << "exception: " << lua_tostring(L, -1);
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

	static int constructor(lua_State* L)
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

namespace channel {

	struct message_queue
	: public std::queue<void*>
	{
	};

	struct mgr {
		std::map<int, message_queue*> m_hmap;
		int                           m_hmax = 0;
		std::mutex                    m_mtx;
		typedef std::scoped_lock<std::mutex> lock;

		std::mutex& mutex() {
			return m_mtx;
		}

		int open() {
			++m_hmax;
			m_hmap[m_hmax] = new message_queue;
			return m_hmax;
		}

		void close(int id) {
			auto it = m_hmap.find(id);
			if (it == m_hmap.end()) {
				return;
			}
			delete it->second;
			m_hmap.erase(it);

		}

		message_queue* get(int id) {
			auto it = m_hmap.find(id);
			if (it == m_hmap.end()) {
				return nullptr;
			}
			return it->second;
		}
	};

	mgr chanmgr;

	static int constructor(lua_State* L)
	{
		mgr::lock lock(chanmgr.mutex());
		lua_pushinteger(L, chanmgr.open());
		return 1;
	}
	static int close(lua_State* L)
	{
		mgr::lock lock(chanmgr.mutex());
		chanmgr.close((int)luaL_checkinteger(L, 1));
		return 0;
	}
	static int send(lua_State* L)
	{
		int id = (int)luaL_checkinteger(L, 1);
		lua_pushcfunction(L, seri_pack);
		lua_replace(L, 1);
		int top = lua_gettop(L);
		lua_call(L, top - 1, 1);
		void * msg = lua_touserdata(L, 1);

		mgr::lock lock(chanmgr.mutex());
		message_queue* mq = chanmgr.get(id);
		if (!mq) {
			return 0;
		}
		mq->push(msg);
		return 0;
	}
	static int recv(lua_State* L)
	{
		void* msg;
		{
			mgr::lock lock(chanmgr.mutex());
			message_queue* mq = chanmgr.get((int)luaL_checkinteger(L, 1));
			if (!mq) {
				return 0;
			}
			if (mq->empty()) {
				lua_pushboolean(L, 0);
				return 1;
			}
			msg = mq->front();
			mq->pop();
		}

		lua_settop(L, 0);
		lua_pushboolean(L, 1);
		lua_pushcfunction(L, seri_unpack);
		lua_pushlightuserdata(L, msg);
		lua_call(L, 1, LUA_MULTRET);
		return lua_gettop(L);
	}
}

extern "C" __declspec(dllexport)
int luaopen_thread(lua_State* L)
{
	static luaL_Reg thread[] = {
		{ "__gc", thread::destructor },
		{ NULL, NULL }
	};
	luaL_newmetatable(L, "thread");
	luaL_setfuncs(L, thread, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	static luaL_Reg api[] = {
		{ "thread", thread::constructor },
		{ "channel", channel::constructor },
		{ "close", channel::close },
		{ "send", channel::send },
		{ "recv", channel::recv },
		{ NULL, NULL }
	};
	luaL_newlib(L, api);
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
