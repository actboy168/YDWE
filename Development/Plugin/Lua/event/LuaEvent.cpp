#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <regex>
#include <bee/utility/path_helper.h>
#include <bee/utility/unicode_win.h>
#include <base/hook/inline.h>
#include <base/hook/iat.h>
#include <base/hook/fp_call.h>	  
#include <base/com/guard.h>	 
#include <base/util/pinyin.h>
#include <lua.hpp>

#include "YDWEEvent.h"
#include "YDWELogger.h"

#include <Mss.h>

namespace NYDWE {

	HFONT font = NULL;

	void lua_pushwstring(lua_State* L, const std::wstring& str)
	{
		std::string ustr = bee::w2u(str);
		lua_pushlstring(L, ustr.data(), ustr.size());
	}

	void lua_pushastring(lua_State* L, const char* str)
	{
		std::string ustr = bee::a2u(str);
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

	static size_t findBuildDir(const std::string& filename) {
		size_t pos = filename.rfind(".w3xTemp");
		if (pos == -1) {
			pos = filename.rfind(".w3mTemp");
			if (pos == -1) {
				pos = filename.rfind(".w3nTemp");
				if (pos == -1) {
					return pos;
				}
			}
		}
		return pos;
	}

	uintptr_t pgTrueFopen;
	FILE* __cdecl DetourWeFopen(const char* filename, const char* mode)
	{
		std::string sFilename(filename);
		size_t pos = findBuildDir(sFilename);
		if (pos == -1) {
			return base::c_call<FILE*>(pgTrueFopen, filename, mode);
		}
		sFilename = sFilename.substr(0, pos) + bee::u2a(sFilename.substr(pos));
		return base::c_call<FILE*>(pgTrueFopen, sFilename.c_str(), mode);
	} 

	uintptr_t pgTrueGetFileAttributesA;
	DWORD WINAPI DetourWeGetFileAttributesA(LPCSTR lpPathName)
	{
		std::string sFilename(lpPathName);
		size_t pos = findBuildDir(sFilename);
		if (pos == -1) {
			return base::std_call<DWORD>(pgTrueGetFileAttributesA, lpPathName);
		}
		sFilename = sFilename.substr(0, pos) + bee::u2a(sFilename.substr(pos));
		return base::std_call<DWORD>(pgTrueGetFileAttributesA, sFilename.c_str());
	}

	uintptr_t pgTrueCreateDirectoryA;
	BOOL WINAPI DetourWeCreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
	{
		std::string sFilename(lpPathName);
		size_t pos = findBuildDir(sFilename);
		if (pos == -1) {
			return base::std_call<BOOL>(pgTrueCreateDirectoryA, lpPathName, lpSecurityAttributes);
		}
		sFilename = sFilename.substr(0, pos) + bee::u2a(sFilename.substr(pos));
		BOOL ok = base::std_call<BOOL>(pgTrueCreateDirectoryA, sFilename.c_str(), lpSecurityAttributes);
		return ok;
	}

	static bool testMap = false;
	static bool saveMap = false;
	bool isWeTestMapHookInstalled;
	uintptr_t pgTrueWeTestMap;
	static int __fastcall DetourWeTestMap(int This)
	{
		testMap = true;
		saveMap = false;
		int res = base::fast_call<int>(pgTrueWeTestMap, This);
		testMap = false;
		saveMap = false;
		return res;
	}

	bool isWeRebuildMapHookInstalled;
	uintptr_t pgTrueWeRebuildMap;
	static int __fastcall DetourWeRebuildMap(int This)
	{
		//int ok = base::fast_call<int>(pgTrueWeRebuildMap, This);
		int unk = *(int*)(This + 12);
		if (unk + 4 < 64) {
			unk = 64;
		}
		else {
			unk = unk + 4;
		}
		int buffer = 0;
		int count = 0;
		int subclass = *(int*)(This + 24);
		const char* mappath = *(const char**)This;
		if (subclass)
		{
			base::fast_call<void>(*(int*)(*(int*)subclass + 32), subclass);
			base::fast_call<void>(*(int*)(*(int*)subclass + 36), subclass, 0, &buffer, &count, 0);
		}
		int map = base::std_call<int>(0x00402B00, mappath, 0x88u, unk, 1, count);
		if (!map) {
			return 0;
		}
		base::std_call<void>(0x004063A0, map, 1, NULL);
		saveMap = true;
		int results = event_array[EVENT_NEW_SAVE_MAP]([&](lua_State* L, int idx) {
			lua_pushstring(L, "map_path");
			lua_pushwstring(L, bee::a2w(mappath));
			lua_rawset(L, idx);

			lua_pushstring(L, "test");
			lua_pushboolean(L, testMap);
			lua_rawset(L, idx);
		});
		return results >= 0 ? 1 : 0;
	}

	/// Regex for extracting file path
	static const std::regex gRegexCommandLine(".*-loadfile\\s+([\\s\\S]+\\.[wW]3[xmXM])");

	uintptr_t pgTrueCreateProcessA;
	BOOL WINAPI DetourWeCreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
	{
		std::cmatch matcher;
		if (lpCommandLine && std::regex_match(lpCommandLine, matcher, gRegexCommandLine))
		{
			fs::path currentWarcraftMap = bee::path_helper::exe_path().parent_path() / matcher.str(1);
			LOGGING_TRACE(lg) << "Executing map " << currentWarcraftMap.wstring();

			int results = event_array[EVENT_TEST_MAP]([&](lua_State* L, int idx){
				lua_pushstring(L, "map_path");
				lua_pushwstring(L, currentWarcraftMap.wstring());
				lua_rawset(L, idx);

				lua_pushstring(L, "save");
				lua_pushboolean(L, saveMap);
				lua_rawset(L, idx);

				if (lpApplicationName) {
					lua_pushstring(L, "application_name");
					lua_pushastring(L, lpApplicationName);
					lua_rawset(L, idx);
				}

				if (lpCommandLine) {
					lua_pushstring(L, "command_line");
					lua_pushastring(L, lpCommandLine);
					lua_rawset(L, idx);
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
		if (strcmp(lpClassName, "LISTBOX") == 0) {
			dwStyle |= LBS_WANTKEYBOARDINPUT;
		}
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

	std::string ListBoxItem(HWND listbox, size_t idx) {
		size_t len = (size_t)SendMessageA(listbox, LB_GETTEXTLEN, (WPARAM)idx, NULL);
		std::string text; text.resize(len);
		SendMessageA(listbox, LB_GETTEXT, (WPARAM)idx, (LPARAM)text.data());
		std::transform(text.begin(), text.end(), text.begin(), ::tolower);
		return base::chinese2pinyin(text);
	}

	bool ListBoxFind(HWND listbox, int i, int j, int& r, char vk) {
		if (i > j) {
			return false;
		}
		int m = (i + j) / 2;
		std::string item = ListBoxItem(listbox, m);
		if (vk == item[0]) {
			for (int k = m - 1; k >= i; --k) {
				std::string item = ListBoxItem(listbox, k);
				if (vk != item[0]) {
					r = k + 1;
					return true;
				}
			}
			r = i;
			return true;
		}
		else if (vk < item[0]) {
			return ListBoxFind(listbox, i, m - 1, r, vk);
		}
		else {
			return ListBoxFind(listbox, m + 1, j, r, vk);
		}
	}

	uintptr_t pgTrueWeDialogProc;
	INT_PTR CALLBACK DetourWeDialogProc(HWND dialogHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_VKEYTOITEM) {
			int current = wParam >> 16;
			if (((wParam & 0xFFFF) < 'A') || ((wParam & 0xFFFF) > 'Z')) {
				return current;
			}
			HWND listbox = (HWND)lParam;
			char vk = tolower(wParam & 0xFF);
			int n = (int)SendMessageA(listbox, LB_GETCOUNT, NULL, NULL);
			if (current >= n) {
				current = n - 1;
			}

			std::string item = ListBoxItem(listbox, current);
			if (vk == item[0]) {
				if (current == n - 1) {
					return current;
				}
				std::string next = ListBoxItem(listbox, current + 1);
				if (next[0] == vk) {
					return current + 1;
				}
				return current;
			}

			int ret = 0;
			if (vk < item[0]) {
				if (!ListBoxFind(listbox, 0, current - 1, ret, vk)) {
					return current;
				}
			}
			else {
				if (!ListBoxFind(listbox, current + 1, n -1, ret, vk)) {
					return current;
				}
			}
			return ret;
		}

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

#define INSTALL_INLINE_HOOK(name) if (!is##name##HookInstalled) { if (pgTrue##name##) { is##name##HookInstalled = base::hook::install(&pgTrue##name##, (uintptr_t)Detour##name##); }}

	void SetupEvent()
	{
		pgTrueFopen           = base::hook::iat(::GetModuleHandleW(NULL), "msvcrt.dll",   "fopen",           (uintptr_t)DetourWeFopen);
		pgTrueGetFileAttributesA = base::hook::iat(::GetModuleHandleW(NULL), "kernel32.dll", "GetFileAttributesA", (uintptr_t)DetourWeGetFileAttributesA);
		pgTrueCreateDirectoryA = base::hook::iat(::GetModuleHandleW(NULL), "kernel32.dll", "CreateDirectoryA", (uintptr_t)DetourWeCreateDirectoryA);
		pgTrueCreateProcessA  = base::hook::iat(::GetModuleHandleW(NULL), "kernel32.dll", "CreateProcessA",  (uintptr_t)DetourWeCreateProcessA);
		pgTrueCreateWindowExA = base::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "CreateWindowExA", (uintptr_t)DetourWeCreateWindowExA);
		pgTrueSetMenu         = base::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "SetMenu",         (uintptr_t)DetourWeSetMenu);
		pgTrueCreateDialogIndirectParamA = base::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "CreateDialogIndirectParamA",  (uintptr_t)DetourWeCreateDialogIndirectParamA);

		pgTrueWeTestMap = (uintptr_t)0x004EB9B0;
		INSTALL_INLINE_HOOK(WeTestMap);

		pgTrueWeRebuildMap = (uintptr_t)0x00402540;
		INSTALL_INLINE_HOOK(WeRebuildMap);
		
		pgTrueWeWinMain     = (uintptr_t)0x004021A0;
		INSTALL_INLINE_HOOK(WeWinMain);

		pgTrueWeNewObjectId = (uintptr_t)0x005B645A;
		INSTALL_INLINE_HOOK(WeNewObjectId)

		pgTrueMssRIBLoadProviderLibrary = (uintptr_t)GetProcAddress(GetModuleHandleA(MSSDLLNAME), "RIB_load_provider_library");
		INSTALL_INLINE_HOOK(MssRIBLoadProviderLibrary);
	}
}
