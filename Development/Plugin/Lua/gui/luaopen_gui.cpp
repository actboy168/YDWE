#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <base/exception/windows_exception.h>  
#include <base/util/unicode.h>
#include <Windows.h>
#include <cstdint>
#include "resource.h"

extern HMODULE self_module;

namespace NLuaAPI { namespace NGUI {

	static void *LuaGuiCreateMenu()
	{
		return CreateMenu();
	}

	static bool LuaGuiAppendMenu(void *hMenu, uint32_t flag, UINT_PTR newItemId, const std::wstring& newItemName)
	{
		return !! AppendMenuW(
			reinterpret_cast<HMENU>(hMenu),
			flag,
			newItemId,
			newItemName.c_str());
	}

	static bool LuaGuiDrawMenuBar(void *hWnd)
	{
		return !!DrawMenuBar(reinterpret_cast<HWND>(hWnd));
	}

	typedef boost::tuple<lua_State *, std::wstring, std::wstring,
		std::wstring, std::wstring, std::wstring> TInputDialogInitInfo;

	struct SInputInfo
	{
		bool isOkClicked;
		wchar_t text[128 * 1024];
	};

	static INT_PTR CALLBACK PromptForInputDialogProc(HWND dialogHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		BOOL result = TRUE;

		// Initialization
		if (message == WM_INITDIALOG)
		{
			// Text initialization
			TInputDialogInitInfo *pInfo = reinterpret_cast<TInputDialogInitInfo *>(lParam);

			SetWindowTextW(dialogHandle, pInfo->get<1>().c_str());
			SetDlgItemTextW(dialogHandle, IDC_PROMPT, pInfo->get<2>().c_str());
			SetDlgItemTextW(dialogHandle, IDC_EDIT_CONTENT, pInfo->get<3>().c_str());
			SetDlgItemTextW(dialogHandle, IDOK, pInfo->get<4>().c_str());
			SetDlgItemTextW(dialogHandle, IDCANCEL, pInfo->get<5>().c_str());
		}
		// Clicked button
		else if (message == WM_COMMAND)
		{
			// ID that triggered
			UINT id = LOWORD(wParam);
			// Clicked "OK" or "Cancel"
			if (id == IDOK || id == IDCANCEL)
			{
				// TODO: Duplicate code.
				SInputInfo *pInfo = reinterpret_cast<SInputInfo *>(malloc(sizeof(SInputInfo)));
				if (pInfo)
				{
					pInfo->isOkClicked = (id == IDOK);
					GetDlgItemTextW(dialogHandle, IDC_EDIT_CONTENT, pInfo->text, sizeof(pInfo->text) / sizeof(pInfo->text[0]));
				}

				EndDialog(dialogHandle, reinterpret_cast<INT_PTR>(pInfo));
			}
		}
		// Clicked close
		else if (message == WM_CLOSE)
		{
			// TODO: Duplicate code.
			SInputInfo *pInfo = reinterpret_cast<SInputInfo *>(malloc(sizeof(SInputInfo)));
			if (pInfo)
			{
				pInfo->isOkClicked = false;
				GetDlgItemTextW(dialogHandle, IDC_EDIT_CONTENT, pInfo->text, sizeof(pInfo->text) / sizeof(pInfo->text[0]));
			}

			EndDialog(dialogHandle, reinterpret_cast<INT_PTR>(pInfo));
		}
		// Messages that not interested
		else
		{
			result = FALSE;
		}

		return result;
	}

	static void LuaGuiPromptForInput(lua_State *pState, void *parentWindowHandle, const std::wstring &caption, const std::wstring &prompt, const std::wstring &initialText, const std::wstring &okCaption, const std::wstring &cancelCaption)
	{
		TInputDialogInitInfo dialogInitInfo(
			pState, caption, prompt, initialText, okCaption, cancelCaption
			);

		SInputInfo *pInputInfo = reinterpret_cast<SInputInfo *>(
			::DialogBoxParamW(self_module, MAKEINTRESOURCEW(IDD_DIALOG_INPUT),
			reinterpret_cast<HWND>(parentWindowHandle), PromptForInputDialogProc,
			reinterpret_cast<LPARAM>(&dialogInitInfo)
			)
			);

		if ((intptr_t)pInputInfo == -1)
		{
			base::windows_exception e;
			luaL_error(pState, e.what());
		}
		else
		{
			luabind::object(pState, (bool)pInputInfo->isOkClicked).push(pState);
			luabind::object(pState, std::wstring(pInputInfo->text)).push(pState);
			free(pInputInfo);
		}
	}

	static void LuaGuiChooseOpenFile(lua_State *pState, void *ownerWindow, const std::wstring &strFilter, const std::wstring &initialFile, const luabind::object &initialDir, const luabind::object &dialogTitle)
	{
		boost::optional<std::wstring> optInitialDir, optTitle;
		wchar_t buffer[MAX_PATH], fileTitle[MAX_PATH];
		OPENFILENAMEW ofn;
		BOOL result;

		memset(&ofn, 0, sizeof(ofn));
		wcscpy_s(buffer, sizeof(buffer), initialFile.c_str());

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = reinterpret_cast<HWND>(ownerWindow);
		ofn.lpstrFilter = strFilter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = buffer;
		ofn.nMaxFile = sizeof(buffer);
		ofn.lpstrFileTitle = fileTitle;
		ofn.nMaxFileTitle = sizeof(fileTitle);
		ofn.lpstrInitialDir = (optInitialDir = luabind::object_cast_nothrow<std::wstring>(initialDir))
			? optInitialDir->c_str()
			: NULL;
		ofn.lpstrTitle = (optTitle = luabind::object_cast_nothrow<std::wstring>(dialogTitle))
			? optTitle->c_str()
			: NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		result = GetOpenFileNameW(&ofn);

		lua_pushboolean(pState, result);
		if (result)
		{
			luabind::object(pState, std::wstring(buffer)).push(pState);
			luabind::object(pState, std::wstring(fileTitle)).push(pState);
		}
		else
		{
			lua_pushnil(pState);
			lua_pushnil(pState);
		}
	}

	static void LuaGuiChooseSaveFile(lua_State *pState, void *ownerWindow, const std::wstring &strFilter, const std::wstring &initialFile, const luabind::object &initialDir, const luabind::object &dialogTitle)
	{
		boost::optional<std::wstring> optInitialDir, optTitle;
		wchar_t buffer[MAX_PATH], fileTitle[MAX_PATH];
		OPENFILENAMEW ofn;
		BOOL result;

		memset(&ofn, 0, sizeof(ofn));
		wcscpy_s(buffer, sizeof(buffer), initialFile.c_str());

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = reinterpret_cast<HWND>(ownerWindow);
		ofn.lpstrFilter = strFilter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = buffer;
		ofn.nMaxFile = sizeof(buffer);
		ofn.lpstrFileTitle = fileTitle;
		ofn.nMaxFileTitle = sizeof(fileTitle);
		ofn.lpstrInitialDir = (optInitialDir = luabind::object_cast_nothrow<std::wstring>(initialDir))
			? optInitialDir->c_str()
			: NULL;
		ofn.lpstrTitle = (optTitle = luabind::object_cast_nothrow<std::wstring>(dialogTitle))
			? optTitle->c_str()
			: NULL;
		ofn.Flags = OFN_PATHMUSTEXIST;

		result = GetSaveFileNameW(&ofn);

		lua_pushboolean(pState, result);
		if (result)
		{
			luabind::object(pState, std::wstring(buffer)).push(pState);
			luabind::object(pState, std::wstring(fileTitle)).push(pState);
		}
		else
		{
			lua_pushnil(pState);
			lua_pushnil(pState);
		}
	}

	static int LuaGuiMessageDialog(void *window, const std::wstring& message, const std::wstring& caption, uint32_t type)
	{
		return MessageBoxW(reinterpret_cast<HWND>(window), message.c_str(), caption.c_str(), type);
	}

	static bool LuaGuiEnableDlgItem(void *dialog, uint32_t item_id, bool enable)
	{
		return !!::EnableWindow(::GetDlgItem(reinterpret_cast<HWND>(dialog), item_id), enable? TRUE: FALSE);
	}

	static void* LuaGuiGetForegroundWindow()
	{
		return (void*)::GetForegroundWindow();
	}
}

static int PointerToNumber(void* p)
{
	return reinterpret_cast<uintptr_t>(p);
}
}

int luaopen_gui(lua_State *pState)
{
	using namespace luabind;

	module(pState, "gui")
	[
		def("create_menu",        &NLuaAPI::NGUI::LuaGuiCreateMenu),
		def("append_menu",        &NLuaAPI::NGUI::LuaGuiAppendMenu),
		def("draw_menu_bar",      &NLuaAPI::NGUI::LuaGuiDrawMenuBar),
		def("choose_open_file",   &NLuaAPI::NGUI::LuaGuiChooseOpenFile),
		def("choose_save_file",   &NLuaAPI::NGUI::LuaGuiChooseSaveFile),
		def("prompt_for_input",   &NLuaAPI::NGUI::LuaGuiPromptForInput),
		def("message_dialog",     &NLuaAPI::NGUI::LuaGuiMessageDialog),
		def("enable_dialog_item", &NLuaAPI::NGUI::LuaGuiEnableDlgItem),
		def("get_foreground_window", &NLuaAPI::NGUI::LuaGuiGetForegroundWindow)
	];

	module(pState, "mem")
	[
		def("pointer_to_number", &NLuaAPI::PointerToNumber)
	];

	lua_getglobal(pState, "gui");
	return 0;
}
