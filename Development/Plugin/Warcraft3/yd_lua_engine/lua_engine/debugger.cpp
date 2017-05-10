#include "debugger.h"

#include <Windows.h>
#include <base/path/self.h>	  
#include <base/warcraft3/event.h>
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
		debugger()
			: m_(L"vscode-debug.dll")
			, handel_(0)
			, create_(0)
			, close_(0)
		{
			if (!m_.handle())
				return;
			create_ = m_.api("vscode_debugger_create");
			close_ = m_.api("vscode_debugger_close");
			attach_lua_ = m_.api("vscode_debugger_attach_lua");
			detach_lua_ = m_.api("vscode_debugger_detach_lua");
			if (!create_ || !close_ || !attach_lua_ || !detach_lua_)
				return;
			handel_ = base::c_call<void*>(create_, "127.0.0.1", 4278, 0);
			if (!handel_)
				return;
		}

		~debugger()
		{
			if (!handel_)
				return;
			base::c_call<void>(close_, handel_);
			handel_ = 0;
		}

		void attach_lua(lua_State* L)
		{
			if (!handel_)
				return;
			if (!attach_lua_)
				return;
			base::c_call<void*>(attach_lua_, handel_, L);
		}

		void detach_lua(lua_State* L)
		{
			if (!handel_)
				return;
			if (!detach_lua_)
				return;
			base::c_call<void*>(detach_lua_, handel_, L);
		}

	private:
		module m_;
		void*  handel_;
		intptr_t create_;
		intptr_t close_;
		intptr_t attach_lua_;
		intptr_t detach_lua_;
	};

	debugger* debugger_create()
	{
		return new debugger();
	}

	void debugger_close(debugger* dbg)
	{
		delete dbg;
	}

	void debugger_attach_lua(debugger* dbg, lua_State* L)
	{
		dbg->attach_lua(L);
	}

	void debugger_detach_lua(debugger* dbg, lua_State* L)
	{
		dbg->detach_lua(L);
	}
}}}
