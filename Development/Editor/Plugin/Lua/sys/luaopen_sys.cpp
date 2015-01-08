
#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>
#include <luabind/operator.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <boost/filesystem.hpp>
#include <Windows.h>
#include <base/win/process.h>
#include <base/win/file_version.h>
#include <fcntl.h>
#include <io.h>

namespace fs = boost::filesystem;

namespace NLuaAPI { namespace NSys {

	namespace detail
	{

		boost::optional<fs::path> cast_path_opt(const luabind::object &object)
		{
			boost::optional<fs::path> path_opt;

			switch (luabind::type(object))
			{
			case LUA_TSTRING:
				{
					boost::optional<std::wstring> str_opt = luabind::object_cast_nothrow<std::wstring>(object);
					if (str_opt)
					{
						path_opt = fs::path(str_opt.get());
					}
				}
				break;
			case LUA_TUSERDATA:
				path_opt = luabind::object_cast_nothrow<fs::path>(object);
				break;
			default:
				break;
			}

			return std::move(path_opt);
		}

		boost::optional<std::wstring> cast_wstring_opt(const luabind::object &object)
		{
			boost::optional<std::wstring> str_opt;

			switch (luabind::type(object))
			{
			case LUA_TSTRING:
				str_opt = luabind::object_cast_nothrow<std::wstring>(object);
				break;
			case LUA_TUSERDATA:
				{
					boost::optional<fs::path> path_opt = luabind::object_cast_nothrow<fs::path>(object);
					if (path_opt)
					{
						str_opt = path_opt->wstring();
					}
				}
				break;
			default:
				break;
			}

			return std::move(str_opt);
		}

		fs::path cast_path(const luabind::object &object)
		{
			boost::optional<fs::path> path_opt = cast_path_opt(object);
			return path_opt ? std::move(path_opt.get()) : std::move(fs::path());
		}

		std::wstring cast_wstring(const luabind::object &object)
		{
			boost::optional<std::wstring> str_opt = cast_wstring_opt(object);
			return str_opt ? std::move(str_opt.get()) : std::move(std::wstring());
		}

		HANDLE cast_file_handle(const luabind::object &object)
		{
			boost::optional<HANDLE> handle_opt;

			switch (luabind::type(object))
			{
			case LUA_TUSERDATA:
				{
					boost::optional<void*> ptr_opt = luabind::object_cast_nothrow<void*>(object);
					if (ptr_opt)
					{
						void* ptr = ptr_opt.get();
						if (ptr)
						{
							handle_opt = (HANDLE)_get_osfhandle(fileno(*(FILE **)ptr));
						}
					}
				}
				break;
			default:
				break;
			}

			return handle_opt ? handle_opt.get() : NULL;
		}
	}

	static void *LuaLoadLibrary(const luabind::object& name_object)
	{
		std::wstring name = detail::cast_wstring(name_object);
		return LoadLibraryW(name.c_str());
	}

	static void LuaUnloadLibrary(void *lib)
	{
		FreeLibrary((HMODULE)lib);
	}

	static void *LuaShellExecute(const std::string &cmd)
	{
		return ShellExecuteA(NULL, 0, cmd.c_str(), 0, 0, SW_SHOW);
	}

	static void LuaGetVersionNumberString(lua_State *pState, const fs::path &module)
	{
		lua_newtable(pState);
		luabind::object version_table(luabind::from_stack(pState, -1));
		base::win::simple_file_version fv(module.c_str());
		version_table["major"]       = fv.major;
		version_table["minor"]       = fv.minor;
		version_table["revision"]    = fv.revision;
		version_table["build"]       = fv.build;
		version_table.push(pState);
	}

	static void LuaSysGetClipboardText(lua_State* pState)
	{
		HANDLE dataHandle;

		if (OpenClipboard(NULL))
		{
			if ((dataHandle = GetClipboardData(CF_TEXT)))
			{
				LPSTR data = (LPSTR)GlobalLock(dataHandle);
				if (data)
					lua_pushstring(pState, data);
				else
					lua_pushnil(pState);

				GlobalUnlock(dataHandle);
			}
			else
			{
				lua_pushnil(pState);
			}

			CloseClipboard();
		}
		else
		{
			lua_pushnil(pState);
		}
	}

	static bool LuaSysSetClipboardText(const std::string &text)
	{
		HANDLE globalMemoryHandle;
		LPSTR pGlobalMemory;
		bool result = false;
		size_t size = text.size();

		if (OpenClipboard(NULL))
		{
			if ((globalMemoryHandle = GlobalAlloc(GHND, size + 1)))
			{
				if ((pGlobalMemory = (LPSTR)GlobalLock(globalMemoryHandle)))
				{
					pGlobalMemory[text.copy(pGlobalMemory, size)] = '\0';
					GlobalUnlock(globalMemoryHandle);
					EmptyClipboard();
					result = !!SetClipboardData(CF_TEXT, globalMemoryHandle);
				}
			}

			CloseClipboard();
		}

		return result;
	}

#define tolstream(L, idx)	((luaL_Stream*)luaL_checkudata(L, idx, LUA_FILEHANDLE))

	static luaL_Stream* LuaNewFile(lua_State* pState) 
	{

		luaL_Stream *p = (luaL_Stream*)lua_newuserdata(pState, sizeof(luaL_Stream));
		p->closef = NULL;
		luaL_setmetatable(pState, LUA_FILEHANDLE);
		return p;
	}

	static int LuaFileClose(lua_State* pState)
	{
		luaL_Stream* p = tolstream(pState, 1);
		int ok = fclose(p->f);
		int en = errno;  /* calls to Lua API may change this value */
		if (ok)
		{
			lua_pushboolean(pState, 1);
			return 1;
		}
		else 
		{
			lua_pushnil(pState);
			lua_pushfstring(pState, "%s", strerror(en));
			lua_pushinteger(pState, en);
			return 3;
		}
	}

	static void LuaOpenFileForHandle(lua_State* pState, HANDLE h, int dmode, const char* mode)
	{
		luaL_Stream* pf = LuaNewFile(pState);
		pf->f      = _fdopen(_open_osfhandle((long)h, dmode), mode);
		pf->closef = &LuaFileClose;
	}
	
	static void LuaOpenPipe(lua_State *pState)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;

		HANDLE read_pipe, write_pipe;
		if (!::CreatePipe(&read_pipe, &write_pipe, &sa, 0))
		{
			lua_pushnil(pState);
			lua_pushnil(pState);
			return ;
		}
		::SetHandleInformation(read_pipe,  HANDLE_FLAG_INHERIT, 0);
		::SetHandleInformation(write_pipe, HANDLE_FLAG_INHERIT, 0);
		LuaOpenFileForHandle(pState, read_pipe,  _O_RDONLY | _O_TEXT, "rt");
		LuaOpenFileForHandle(pState, write_pipe, _O_WRONLY | _O_TEXT, "wt");
	}

	static void LuaProcessCreate(lua_State *pState, base::win::process& p, const luabind::object &application_object, const luabind::object &commandline_object, const luabind::object &currentdirectory_object)
	{
		boost::optional<fs::path> application_opt      = detail::cast_path_opt(application_object);
		std::wstring              commandline          = detail::cast_wstring(commandline_object);
		boost::optional<fs::path> currentdirectory_opt = detail::cast_path_opt(currentdirectory_object);

		bool result = false;
		if (application_opt)
		{
			if (currentdirectory_opt)
			{
				result = p.create(application_opt.get(), commandline, currentdirectory_opt.get());
			}
			else
			{
				result = p.create(application_opt.get(), commandline);
			}
		}
		else
		{
			if (currentdirectory_opt)
			{
				result = p.create(commandline, currentdirectory_opt.get());
			}
			else
			{
				result = p.create(commandline);
			}
		}

		lua_pushboolean(pState, result);
	}

	static void LuaProcessRedirect(lua_State *pState, base::win::process& p, const luabind::object &stdinput_object, const luabind::object &stdoutput_object, const luabind::object &stderror_object)
	{
		HANDLE stdinput  = detail::cast_file_handle(stdinput_object);
		HANDLE stdoutput = detail::cast_file_handle(stdoutput_object);
		HANDLE stderror  = detail::cast_file_handle(stderror_object);
		bool result = p.redirect(stdinput, stdoutput, stderror);
		lua_pushboolean(pState, result);
	}
}}

int luaopen_sys(lua_State *pState)
{
	using namespace luabind;

	// Bind sys
	module(pState, "sys")
	[
		class_<base::win::process>("process")
			.def(constructor<>())
			.def("inject",      &base::win::process::inject)
			.def("hide_window", &base::win::process::hide_window)
			.def("redirect",    &NLuaAPI::NSys::LuaProcessRedirect)
			.def("create",      &NLuaAPI::NSys::LuaProcessCreate)
			.def("wait",        (uint32_t (base::win::process::*)())&base::win::process::wait)
			.def("close",       &base::win::process::close)
		,

		def("open_pipe", &NLuaAPI::NSys::LuaOpenPipe),
		def("load_library", &NLuaAPI::NSys::LuaLoadLibrary),
		def("unload_library", &NLuaAPI::NSys::LuaUnloadLibrary),
		def("shell_execute", &NLuaAPI::NSys::LuaShellExecute),
		def("get_module_version_info", &NLuaAPI::NSys::LuaGetVersionNumberString),
		def("get_clipboard_text", &NLuaAPI::NSys::LuaSysGetClipboardText),
		def("set_clipboard_text", &NLuaAPI::NSys::LuaSysSetClipboardText)
	];

	return 0;
}
