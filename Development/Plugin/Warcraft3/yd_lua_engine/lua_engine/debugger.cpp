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
		module(const fs::path& path)
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

	private:
		HMODULE handle_;
	};

	class debugger
	{
	public:
		debugger(lua_State* L)
			: m_(L"vscode-debug.dll")
			, handel_(0)
			, create_(0)
			, close_(0)
			, update_(0)
		{
			if (!m_.handle())
				return;
			create_ = m_.api("vscode_debugger_create");
			close_ = m_.api("vscode_debugger_close");
			update_ = m_.api("vscode_debugger_update");
			if (!create_ || !close_ || !update_)
				return;
			handel_ = base::c_call<void*>(create_, L, "127.0.0.1", 4278);
			if (!handel_)
				return;
			jass::jreal_t t = jass::to_real(0.1f);
			jass::call("TimerStart", jass::call("CreateTimer"), &t, true, jass::trampoline_create(debugger::static_update, (uint32_t)(uintptr_t)this, 0));
		}

		~debugger()
		{
			if (!handel_)
				return;
			base::c_call<void>(close_, handel_);
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
		intptr_t create_;
		intptr_t close_;
		intptr_t update_;
	};

	debugger* debugger_create(lua_State* L)
	{
		return new debugger(L);
	}

	void debugger_close(debugger* dbg)
	{
		delete dbg;
	}
}}}
