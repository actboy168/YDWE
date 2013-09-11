#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/scope_exit.hpp>
#include <aero/aero.hpp>
#include <base/path/service.h>
#include <base/util/unicode.h>
#include <base/hook/inline.h>
#include <base/hook/iat.h>

#include "YDWEEvent.h"
#include "YDWELogger.h"

#include <Mss.h>

namespace NYDWE {

	HWND  gWeMainWindowHandle;
	HMENU gWeMainMenuHandle;

	bool isWeWinMainHookInstalled;
	uintptr_t pgTrueWeWinMain;
	boost::int32_t AERO_FP_CALL_STDCALL DetourWeWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, boost::int32_t showCommand)
	{
		LOG4CXX_INFO(NYDWE::gInjectLogger, "Entering main program.");
		LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Command line: " << (commandLine ? commandLine : "NULL"));

		// Initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		BOOST_SCOPE_EXIT( (hr) )
		{
			if (SUCCEEDED(hr))
				CoUninitialize();
		} BOOST_SCOPE_EXIT_END

			CYDWEEventData eventData;
		event_array[EVENT_WE_START](eventData);

		boost::int32_t result = aero::std_call<boost::int32_t>(pgTrueWeWinMain, instance, prevInstance, commandLine, showCommand);

		eventData.getDataStore().clear();
		event_array[EVENT_WE_EXIT](eventData);

		LOG4CXX_INFO(NYDWE::gInjectLogger, "Main program exit.");

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
		std::string fileName(lpFileName);

		if (gIsInCompileProcess && (
			boost::iends_with(fileName, ".w3x") ||
			boost::iends_with(fileName, ".w3m")/* ||
											   boost::iends_with(fileName, ".w3n") */ // Disabled because does not support campaign
											   ))
		{
			CYDWEEventData eventData;
			eventData.setEventData("map_path", fileName);
			event_array[EVENT_SAVE_MAP](eventData);

			gIsInCompileProcess = false;
		}

		//LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Storm CreateFile: %1%") % lpFileName);
		return aero::std_call<HANDLE>(pgTrueCreateFileA, lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	HANDLE WINAPI DetourWeCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
	{
		std::string fileName(lpFileName);
		if (boost::iends_with(fileName, "war3map.j"))
		{
			LOG4CXX_TRACE(NYDWE::gInjectLogger, "WE is about to compile maps.");
			gIsInCompileProcess = true;
		}

		//LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("WE CreateFile: %1%") % lpFileName);
		return aero::std_call<HANDLE>(pgTrueCreateFileA, lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	/// Regex for extracting file path
	static const std::regex gRegexCommandLine(".*-loadfile\\s+([\\s\\S]+\\.[wW]3[xmXM])");

	uintptr_t pgTrueCreateProcessA;
	BOOL WINAPI DetourWeCreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
	{
		//LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("ApplicationName: %1%") % lpApplicationName);
		//LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("CommandLine: %1%") % lpCommandLine);

		std::cmatch matcher;
		if (lpCommandLine && std::regex_match(lpCommandLine, matcher, gRegexCommandLine))
		{
			std::string currentWarcraftMap = (ydwe::path::get(ydwe::path::DIR_EXE).remove_filename() / matcher.str(1)).string();
			LOG4CXX_TRACE(NYDWE::gInjectLogger, "Executing map " + currentWarcraftMap);

			CYDWEEventData eventData;
			if (gIsInCompileProcess)
			{
				LOG4CXX_TRACE(NYDWE::gInjectLogger, "Need to compile...");

				eventData.setEventData("map_path", currentWarcraftMap);

				const std::vector<int> &results = event_array[EVENT_SAVE_MAP](eventData);
				gIsInCompileProcess = false;
				if (results_is_failed(results))
				{
					LOG4CXX_TRACE(NYDWE::gInjectLogger, "Save failed. Abort testing.");
					memset(lpProcessInformation, 0, sizeof(PROCESS_INFORMATION));
					return FALSE;
				}
			}
			else
			{
				LOG4CXX_TRACE(NYDWE::gInjectLogger, "No need to compile.");
			}

			eventData.getDataStore().clear();
			eventData.setEventData("map_path", currentWarcraftMap);
			if (lpApplicationName)
				eventData.setEventData("application_name", std::string(lpApplicationName));
			if (lpCommandLine)
				eventData.setEventData("command_line", std::string(lpCommandLine));

			const std::vector<int> &results = event_array[EVENT_TEST_MAP](eventData);
			return (!results_is_failed(results));
		}
		else
		{
			// Retain original
			return aero::std_call<BOOL>(pgTrueCreateProcessA,
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
		CYDWEEventData eventData;
		eventData.setEventData("handle", windowHandle);
		eventData.setEventData("message", message);
		eventData.setEventData("wparam", wParam);
		eventData.setEventData("lparam", lParam);

		const std::vector<int> &results = event_array[EVENT_WINDOW_MESSAGE](eventData);
		if (!results_is_failed(results))
		{
			// All allowed
			return aero::std_call<LRESULT>(pgTrueWeWindowProc, windowHandle, message, wParam, lParam);
		}
		else
		{
			return 0;
		}
	}

	uintptr_t pgTrueCreateWindowExA;
	HWND WINAPI DetourWeCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		HWND result = aero::std_call<HWND>(pgTrueCreateWindowExA,
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

		if (strcmp("Warcraft III", lpClassName) == 0)
		{
			gWeMainWindowHandle = result;
		}

		return result;
	}

	uintptr_t pgTrueSetMenu;
	BOOL WINAPI DetourWeSetMenu(HWND hWnd, HMENU hMenu)
	{
		if (hWnd == gWeMainWindowHandle)
		{
			// Main window menu
			gWeMainMenuHandle = hMenu;

			// Call menu initialization
			CYDWEEventData eventData;
			eventData.setEventData("main_window_handle", hWnd);
			eventData.setEventData("main_menu_handle", hMenu);
			event_array[EVENT_INIT_MENU](eventData);

			// Hook window
			pgTrueWeWindowProc = (decltype(pgTrueWeWindowProc))GetWindowLongPtrA(gWeMainWindowHandle, GWL_WNDPROC);
			SetWindowLongA(gWeMainWindowHandle, GWL_WNDPROC, reinterpret_cast<LONG>(DetourWeWindowProc));
		}

		return aero::std_call<BOOL>(pgTrueSetMenu, hWnd, hMenu);
	}

	uintptr_t pgTrueWeDialogProc;
	INT_PTR CALLBACK DetourWeDialogProc(HWND dialogHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_SETTEXT)
		{
			if (lParam)
			{
				CYDWEEventData eventData;
				eventData.setEventData("handle", dialogHandle);
				eventData.setEventData("message", message);
				eventData.setEventData("wparam", wParam);
				eventData.setEventData("lparam", std::string((const char*)lParam));
				const std::vector<int> &results = event_array[EVENT_DIALOG_MESSAGE](eventData);

				if (results_is_failed(results))
				{
					return 0;
				}
			}
		}
		else if (message == WM_COMMAND)
		{
			CYDWEEventData eventData;
			eventData.setEventData("handle", dialogHandle);
			eventData.setEventData("message", message);
			eventData.setEventData("wparam", wParam);
			eventData.setEventData("lparam", lParam);
			const std::vector<int> &results = event_array[EVENT_DIALOG_MESSAGE](eventData);

			if (results_is_failed(results))
			{
				return 0;
			}
		}

		return aero::std_call<INT_PTR>(pgTrueWeDialogProc, dialogHandle, message, wParam, lParam);
	}

	uintptr_t pgTrueCreateDialogIndirectParamA;
	HWND WINAPI DetourWeCreateDialogIndirectParamA(HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit)
	{
		if (!pgTrueWeDialogProc)
		{
			pgTrueWeDialogProc = (uintptr_t)lpDialogFunc;
		}

		return aero::std_call<HWND>(pgTrueCreateDialogIndirectParamA, hInstance, lpTemplate, hWndParent, DetourWeDialogProc, lParamInit);
	}

	uintptr_t pgTrueWeNewObjectId;
	bool isWeNewObjectIdHookInstalled;

	int32_t __fastcall PromptUserForNewId(uintptr_t this_, int32_t default_id)
	{
		uint32_t object_type = ((uint32_t(__fastcall*)(uintptr_t))(*(uintptr_t*)(*(uintptr_t*)this_ + 0x18)))(this_);

		CYDWEEventData eventData;
		eventData.setEventData("class", this_);
		eventData.setEventData("object_type", object_type);
		eventData.setEventData("default_id", default_id);
		const std::vector<int> &results = event_array[EVENT_NEW_OBJECT_ID](eventData);

		if (results_is_failed(results))
		{
			return default_id;
		}

		return results.size() > 0 ? results[0] : default_id;
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
		CYDWEEventData eventData;
		eventData.setEventData("library_name", std::string(fileName));
		const std::vector<int> &results = event_array[EVENT_MSS_LOAD](eventData);

		// If all of the results are allowed, load it
		if (!results_is_failed(results))
			return aero::std_call<HPROVIDER>(pgTrueMssRIBLoadProviderLibrary, fileName);
		else
			return 0;
	}

#define INSTALL_INLINE_HOOK(name) if (!is##name##HookInstalled) { if (pgTrue##name##) { is##name##HookInstalled = ydwe::hook::inline_install(&pgTrue##name##, (uintptr_t)Detour##name##); }}

	void SetupEvent()
	{
		pgTrueCreateFileA     = ydwe::hook::iat(L"storm.dll",             "kernel32.dll", "CreateFileA",     (uintptr_t)DetourStormCreateFileA);
		pgTrueCreateFileA     = ydwe::hook::iat(::GetModuleHandleW(NULL), "kernel32.dll", "CreateFileA",     (uintptr_t)DetourWeCreateFileA);
		pgTrueCreateProcessA  = ydwe::hook::iat(::GetModuleHandleW(NULL), "kernel32.dll", "CreateProcessA",  (uintptr_t)DetourWeCreateProcessA);
		pgTrueCreateWindowExA = ydwe::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "CreateWindowExA", (uintptr_t)DetourWeCreateWindowExA);
		pgTrueSetMenu         = ydwe::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "SetMenu",         (uintptr_t)DetourWeSetMenu);
		pgTrueCreateDialogIndirectParamA = ydwe::hook::iat(::GetModuleHandleW(NULL), "user32.dll",   "CreateDialogIndirectParamA",  (uintptr_t)DetourWeCreateDialogIndirectParamA);

		pgTrueWeWinMain     = (uintptr_t)0x004021A0;
		INSTALL_INLINE_HOOK(WeWinMain);

		pgTrueWeNewObjectId = (uintptr_t)0x005B645A;
		INSTALL_INLINE_HOOK(WeNewObjectId)

		pgTrueMssRIBLoadProviderLibrary = (uintptr_t)GetProcAddress(GetModuleHandleA(MSSDLLNAME), "RIB_load_provider_library");
		INSTALL_INLINE_HOOK(MssRIBLoadProviderLibrary);
	}
}
