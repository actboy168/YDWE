#include "debugger.h"

#include <Windows.h>
#include <base/path/self.h>	  
#include <base/warcraft3/event.h>	 
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/trampoline_function.h>  
#include <base/hook/fp_call.h>
#include <lua.hpp>

namespace base { namespace warcraft3 { namespace lua_engine {

	class module {
	public:
		module(const boost::filesystem::path& path)
			: handle_(LoadLibraryW(path.c_str()))
		{ }

		HMODULE handle() const
		{
			return handle_;
		}

		intptr_t api(const char* name)
		{
			return (intptr_t)GetProcAddress(handle_, name);
		}

		template <class R, class ... Args>
		R c_call(const char* name, const Args& ...args)
		{
			intptr_t fp = api(name);
			if (!fp) {
				throw std::runtime_error(name);
			}
			return base::c_call<R>(fp, args ...);
		}

	private:
		HMODULE handle_;
	};

	class debugger
	{
	public:
		debugger(lua_State* L)
			: m_(base::path::self().remove_filename().remove_filename().remove_filename() / L"bin" / L"vscode-lua-debug.dll")
			, handel_(0)
			, update_(0)
		{
			if (!m_.handle())
				return;
			handel_ = m_.c_call<void*>("vscode_debugger_create", L, "127.0.0.1", 4278);
			if (!handel_)
				return;
			update_ = m_.api("vscode_debugger_update");

			jass::jreal_t t = jass::to_real(0.1f);
			jass::call("TimerStart", jass::call("CreateTimer"), &t, true, jass::trampoline_create(debugger::static_update, (uint32_t)(uintptr_t)this, 0));
		}

		~debugger()
		{
			if (!handel_)
				return;
			m_.c_call<void>("vscode_debugger_close", handel_);
			handel_ = 0;
		}

		void update()
		{
			if (!handel_)
				return;
			if (!update_)
				return;
			base::c_call<void>(update_, handel_);
		}

		static uint32_t __fastcall static_update(uint32_t dbg, uint32_t)
		{
			((debugger*)dbg)->update();
			return 0;
		}

	private:
		module m_;
		void*  handel_;
		intptr_t update_;
	};

	void debugger_init(lua_State* L)
	{
		debugger* dbg = new debugger(L);
		register_game_reset_event([&](uintptr_t)
		{
			delete dbg;
			dbg = 0;
		});
	}
}}}
