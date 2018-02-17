#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <regex>
#include <base/path/service.h>
#include <base/util/unicode.h>
#include <base/hook/inline.h>
#include <base/hook/iat.h>
#include <base/hook/fp_call.h>	  
#include <base/com/guard.h>	 
#include <lua.hpp>

#include "YDWEEvent.h"
#include "YDWELogger.h"

#include <Mss.h>

namespace NYDWE {

	HFONT font = NULL;

	void lua_pushwstring(lua_State* L, const std::wstring& str)
	{
		std::string ustr = base::w2u(str, base::conv_method::replace | '?');
		lua_pushlstring(L, ustr.data(), ustr.size());
	}

	void lua_pushastring(lua_State* L, const char* str)
	{
		std::string ustr = base::a2u(str, base::conv_method::replace | '?');
		lua_pushlstring(L, ustr.data(), ustr.size());
	}
	
	TEvent event_array[EVENT_MAXIMUM];

	HWND  gWeMainWindowHandle;
	HMENU gWeMainMenuHandle;

	bool isWeWinMainHookInstalled;
	uintptr_t pgTrueWeWinMain;
	int32_t WINAPI DetourWeWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int32_t showCommand)
	{
		LOGGING_INFO(lg) << "Entering main program.";

		// Initialize COM	   
		base::com::guard com;

		event_array[EVENT_WE_START]([&](lua_State* L, int idx){
		});

		int32_t result = base::std_call<int32_t>(pgTrueWeWinMain, instance, prevInstance, commandLine, showCommand);

		event_array[EVENT_WE_EXIT]([&](lua_State* L, int idx){

		});

		LOGGING_INFO(lg) << "Main program exit.";

		// Finish engine here to prevent .net module crash
		// Before we stop the lua engine, we should stop hook
		// UninstallHooks();

		//ShutdownLua();

		return result;
	}

	volatile bool gIsInCompileProcess = false;
	uintptr_t pgTrueCreateFileA;
	HANDLE WINAPI DetourStormCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		std::wstring fileName = base::a2w(std::string_view(lpFileName), base::conv_method::replace | '?');
		std::wstring_view fileExt(fileName.data() + fileName.size() - 4, 4);
		if (gIsInCompileProcess && (fileExt == L".w3x" || fileExt == L".w3m"))
		{
			gIsInCompileProcess = false;
			event_array[EVENT_SAVE_MAP]([&](lua_State* L, int idx){
				lua_pushstring(L, "map_path");
				lua_pushwstring(L, fileName);
				lua_settable(L, idx);
			});
		}

		return base::std_call<HANDLE>(pgTrueCreateFileA, lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	HANDLE WINAPI DetourWeCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		std::string fileName(lpFileName);
		std::string_view fileExt(fileName.data() + fileName.size() - 4, 4);
		if (std::string_view(fileName.data() + fileName.size() - 14, 14) == "war3mapMap.blp")
		{
			if (dwCreationDisposition == OPEN_EXISTING)
			{
				LOGGING_TRACE(lg) << "WE is about to compile maps.";
				gIsInCompileProcess = true;
			}
			else
			{
				gIsInCompileProcess = false;
			}
		}
		else if (gIsInCompileProcess && (fileExt == ".w3x" || fileExt == ".w3m"))
		{
			try {
				fs::path p(fileName);
				//p = p.parent_path().remove_filename() / p.filename();

				event_array[EVENT_SAVE_MAP]([&](lua_State* L, int idx){
					lua_pushstring(L, "map_path");
					lua_pushwstring(L, p.wstring());
					lua_settable(L, idx);
				});
			}
			catch (...) {				
			}
		}

		return base::std_call<HANDLE>(pgTrueCreateFileA, lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	/// Regex for extracting file path
	static const std::regex gRegexCommandLine(".*-loadfile\\s+([\\s\\S]+\\.[wW]3[xmXM])");

	uintptr_t pgTrueCreateProcessA;
	BOOL WINAPI DetourWeCreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
	{
		std::cmatch matcher;
		if (lpCommandLine && std::regex_match(lpCommandLine, matcher, gRegexCommandLine))
		{
			fs::path currentWarcraftMap = base::path::get(base::path::DIR_EXE).remove_filename() / matcher.str(1);
			LOGGING_TRACE(lg) << "Executing map " << currentWarcraftMap.wstring();

			if (gIsInCompileProcess)
			{
				LOGGING_TRACE(lg) << "Need to compile...";

				int results = event_array[EVENT_SAVE_MAP]([&](lua_State* L, int idx){
					lua_pushstring(L, "map_path");
					lua_pushwstring(L, currentWarcraftMap.wstring());
					lua_settable(L, idx);
				});

				gIsInCompileProcess = false;
				if (results < 0)
				{
					LOGGING_TRACE(lg) << "Save failed. Abort testing.";
					memset(lpProcessInformation, 0, sizeof(PROCESS_INFORMATION));
					return FALSE;
				}
			}
			else
			{
				LOGGING_TRACE(lg) << "No need to compile.";
			}

			int results = event_array[EVENT_TEST_MAP]([&](lua_State* L, int idx){
				lua_pushstring(L, "map_path");
				lua_pushwstring(L, currentWarcraftMap.wstring());
				lua_settable(L, idx);

				if (lpApplicationName) {
					lua_pushstring(L, "application_name");
					lua_pushastring(L, lpApplicationName);
					lua_settable(L, idx);
				}

				if (lpCommandLine) {
					lua_pushstring(L, "command_line");
					lua_pushastring(L, lpCommandLine);
					lua_settable(L, idx);
				}
			});
			return results >= 0;
		}
		else
		{
			// Retain original
			return base::std_call<BOOL>(pgTrueCreateProcessA,
				lpApplicationName,
				lpCommandLine,
				lpProcessAttributes,
				lpThreadAttributes,
				bInheritHandles,
				dwCreationFlags,
				lpEnvironment,
				lpCurrentDirectory,
				lpStartupInfo,
				lpProcessInformation
				);
		}
	}


	uintptr_t pgTrueWeWindowProc;
	LRESULT CALLBACK DetourWeWindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message != WM_COMMAND) {
			return base::std_call<LRESULT>(pgTrueWeWindowProc, windowHandle, message, wParam, lParam);
		}
		int results = event_array[EVENT_WINDOW_MESSAGE]([&](lua_State* L, int idx){
			lua_pushstring(L, "handle");
			lua_pushinteger(L, (lua_Integer)windowHandle);
			lua_settable(L, idx);
			lua_pushstring(L, "message");
			lua_pushinteger(L, (lua_Integer)message);
			lua_settable(L, idx);
			lua_pushstring(L, "wparam");
			lua_pushinteger(L, (lua_Integer)wParam);
			lua_settable(L, idx);
			lua_pushstring(L, "lparam");
			lua_pushinteger(L, (lua_Integer)lParam);
			lua_settable(L, idx);
		});

		if (results < 0)
		{
			return 0;
		}
		return base::std_call<LRESULT>(pgTrueWeWindowProc, windowHandle, message, wParam, lParam);
	}

	uintptr_t pgTrueCreateWindowExA;
	HWND WINAPI DetourWeCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		HWND result = base::std_call<HWND>(pgTrueCreateWindowExA,
			dwExStyle,
			lpClassName,
			lpWindowName,
			dwStyle,
			X,
			Y,
			nWidth,
			nHeight,
			hWndParent,
			hMenu,
			hInstance,
			lpParam
			);

		if (result) {
			if (strcmp("Warcraft III", lpClassName) == 0) {
				gWeMainWindowHandle = result;
			}
			if (font) {
				::PostMessage(result, WM_SETFONT, (WPARAM)(HFONT)(font), (LPARAM)(BOOL)(0));
			}
		}
		return result;
	}

	uintptr_t pgTrueSetMenu;
	BOOL WINAPI DetourWeSetMenu(HWND hWnd, HMENU hMenu)
	{
		if (hWnd == gWeMainWindowHandle)
		{
			gWeMainMenuHandle = hMenu;
			int results = event_array[EVENT_INIT_MENU]([&](lua_State* L, int idx){
				lua_pushstring(L, "main_window_handle");
				lua_pushinteger(L, (lua_Integer)hWnd);
				lua_settable(L, idx);
				lua_pushstring(L, "main_menu_handle");
				lua_pushinteger(L, (lua_Integer)hMenu);
				lua_settable(L, idx);
			});

			// Hook window
			pgTrueWeWindowProc = (decltype(pgTrueWeWindowProc))GetWindowLongPtrA(gWeMainWindowHandle, GWL_WNDPROC);
			SetWindowLongA(gWeMainWindowHandle, GWL_WNDPROC, reinterpret_cast<LONG>(DetourWeWindowProc));
		}

		return base::std_call<BOOL>(pgTrueSetMenu, hWnd, hMenu);
	}

	uintptr_t pgTrueWeDialogProc;
	INT_PTR CALLBACK DetourWeDialogProc(HWND dialogHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		INT_PTR ret = base::std_call<INT_PTR>(pgTrueWeDialogProc, dialogHandle, message, wParam, lParam);

		if (message == WM_SETTEXT)
		{
			if (lParam)
			{
				int results = event_array[EVENT_DIALOG_MESSAGE]([&](lua_State* L, int idx){
					lua_pushstring(L, "handle");
					lua_pushinteger(L, (lua_Integer)dialogHandle);
					lua_settable(L, idx);
					lua_pushstring(L, "message");
					lua_pushinteger(L, (lua_Integer)message);
					lua_settable(L, idx);
					lua_pushstring(L, "wparam");
					lua_pushinteger(L, (lua_Integer)wParam);
					lua_settable(L, idx);
					lua_pushstring(L, "lparam");
					lua_pushastring(L, (const char*)lParam);
					lua_settable(L, idx);
				});
				if (results < 0)
				{
					return 0;
				}
			}
		}
		else if (message == WM_COMMAND)
		{
			int results = event_array[EVENT_DIALOG_MESSAGE]([&](lua_State* L, int idx){
				lua_pushstring(L, "handle");
				lua_pushinteger(L, (lua_Integer)dialogHandle);
				lua_settable(L, idx);
				lua_pushstring(L, "message");
				lua_pushinteger(L, (lua_Integer)message);
				lua_settable(L, idx);
				lua_pushstring(L, "wparam");
				lua_pushinteger(L, (lua_Integer)wParam);
				lua_settable(L, idx);
				lua_pushstring(L, "lparam");
				lua_pushinteger(L, (lua_Integer)lParam);
				lua_settable(L, idx);
			});
			if (results < 0)
			{
				return 0;
			}
		}

		return ret;
	}

	uintptr_t pgTrueCreateDialogIndirectParamA;
	HWND WINAPI DetourWeCreateDialogIndirectParamA(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit)
	{
		if (!pgTrueWeDialogProc)
		{
			pgTrueWeDialogProc = (uintptr_t)lpDialogFunc;
		}

		return base::std_call<HWND>(pgTrueCreateDialogIndirectParamA, hInstance, lpTemplate, hWndParent, DetourWeDialogProc, lParamInit);
	}

	uintptr_t pgTrueWeNewObjectId;
	bool isWeNewObjectIdHookInstalled;

	int32_t __fastcall PromptUserForNewId(uintptr_t this_, int32_t default_id)
	{
		uint32_t object_type = ((uint32_t(__fastcall*)(uintptr_t))(*(uintptr_t*)(*(uintptr_t*)this_ + 0x18)))(this_);

		int results = event_array[EVENT_NEW_OBJECT_ID]([&](lua_State* L, int idx){
			lua_pushstring(L, "class");
			lua_pushinteger(L, (lua_Integer)this_);
			lua_settable(L, idx);
			lua_pushstring(L, "object_type");
			lua_pushinteger(L, (lua_Integer)object_type);
			lua_settable(L, idx);
			lua_pushstring(L, "default_id");
			lua_pushinteger(L, (lua_Integer)default_id);
			lua_settable(L, idx);
		});
		if (results < 0)
		{
			return default_id;
		}
		return results;
	}

	static void __declspec(naked) DetourWeNewObjectId()
	{
		__asm
		{
			push	eax;
			push	ecx;
			mov		ecx, ebx
			call	PromptUserForNewId;
			mov		edx, eax
			pop		ecx;
			pop		eax;
			mov     [eax+4], edx;
			mov     [eax], ecx;
			mov     dword ptr [ebx+24h], 1;
			mov     eax, edx;
			pop     ebx;
			mov     esp, ebp;
			pop     ebp;
			retn    4;
		}
	}

	bool isMssRIBLoadProviderLibraryHookInstalled;
	uintptr_t pgTrueMssRIBLoadProviderLibrary;
	static HPROVIDER AILCALL DetourMssRIBLoadProviderLibrary(C8 const FAR *fileName)
	{
		int results = event_array[EVENT_MSS_LOAD]([&](lua_State* L, int idx){
			lua_pushstring(L, "library_name");
			lua_pushastring(L, fileName);
			lua_settable(L, idx);
		});
		if (results < 0)
			return 0;
		return base::std_call<HPROVIDER>(pgTrueMssRIBLoadProviderLibrary, fileName);
	}

#define INSTALL_INLINE_HOOK(name) if (!is##name##HookInstalled) { if (pgTrue##name##) { is##name##HookInstalled = base::hook::inline_install(&pgTrue##name##, (uintptr_t)Detour##name##); }}

	void SetupEvent()
	{
		pgTrueCreateFileA     = base::hook::iat(L"storm.dll",             "kernel32.dll", "CreateFileA",     (uintptr_t)DetourStormCreateFileA);
		pgTrueCreateFileA     = base::hook::iat(::GetModuleHandleW(NULL), "kernel32.dll", "CreateFileA",     (uintptr_t)DetourWeCreateFileA);
		pgTrueCreateProcessA  = base::hook::iat(::GetModuleHandleW(NULL), "kernel32.dll", "CreateProcessA",  (uintptr_t)DetourWeCreateProcessA);
		pgTrueCreateWindowExA = base::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "CreateWindowExA", (uintptr_t)DetourWeCreateWindowExA);
		pgTrueSetMenu         = base::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "SetMenu",         (uintptr_t)DetourWeSetMenu);
		pgTrueCreateDialogIndirectParamA = base::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "CreateDialogIndirectParamA",  (uintptr_t)DetourWeCreateDialogIndirectParamA);

		pgTrueWeWinMain     = (uintptr_t)0x004021A0;
		INSTALL_INLINE_HOOK(WeWinMain);

		pgTrueWeNewObjectId = (uintptr_t)0x005B645A;
		INSTALL_INLINE_HOOK(WeNewObjectId)

		pgTrueMssRIBLoadProviderLibrary = (uintptr_t)GetProcAddress(GetModuleHandleA(MSSDLLNAME), "RIB_load_provider_library");
		INSTALL_INLINE_HOOK(MssRIBLoadProviderLibrary);
	}
}
