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

	static bool LuaGuiInsertMenu(void *hMenu, uint32_t position, uint32_t flag, UINT_PTR newItemId, const std::wstring& newItemName)
	{
		return !! InsertMenuW(
			reinterpret_cast<HMENU>(hMenu),
			position,
			flag,
			newItemId,
			newItemName.c_str());
	}

	static bool LuaGuiModifyMenu(void *hMenu, uint32_t position, uint32_t flag, UINT_PTR newItemId, const std::wstring& newItemName)
	{
		return !! ModifyMenuW(
			reinterpret_cast<HMENU>(hMenu),
			position,
			flag,
			newItemId,
			newItemName.c_str());
	}

	static bool LuaGuiDeleteMenu(void *hMenu, uint32_t position, uint32_t flag)
	{
		return !! DeleteMenu(
			reinterpret_cast<HMENU>(hMenu),
			position,
			flag);
	}

	static bool LuaGuiRemoveMenu(void *hMenu, uint32_t position, uint32_t flag)
	{
		return !! RemoveMenu(
			reinterpret_cast<HMENU>(hMenu),
			position,
			flag);
	}

	static bool LuaGuiDrawMenuBar(void *hWnd)
	{
		return !!DrawMenuBar(reinterpret_cast<HWND>(hWnd));
	}

	static uint32_t LuaGuiCheckMenuItem(void *hMenu, uint32_t id, uint32_t check)
	{
		return CheckMenuItem(
			reinterpret_cast<HMENU>(hMenu),
			id,
			check
			);
	}

	static bool LuaGuiEnableMenuItem(void *hMenu, uint32_t id, uint32_t enable)
	{
		return !! EnableMenuItem(
			reinterpret_cast<HMENU>(hMenu),
			id,
			enable);
	}

	static uint32_t LuaGuiGetMenuItemId(void *hMenu, uint32_t position)
	{
		return GetMenuItemID(
			reinterpret_cast<HMENU>(hMenu),
			position
			);
	}

	static void *LuaGuiGetMenu(void *hWnd)
	{
		return GetMenu(reinterpret_cast<HWND>(hWnd));
	}

	static void *LuaGuiGetSubMenu(void *hMenu, int32_t position)
	{
		return GetSubMenu(reinterpret_cast<HMENU>(hMenu), position);
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
		def("get_menu",           &NLuaAPI::NGUI::LuaGuiGetMenu),
		def("get_sub_menu",       &NLuaAPI::NGUI::LuaGuiGetSubMenu),
		def("append_menu",        &NLuaAPI::NGUI::LuaGuiAppendMenu),
		def("insert_menu",        &NLuaAPI::NGUI::LuaGuiInsertMenu),
		def("modify_menu",        &NLuaAPI::NGUI::LuaGuiModifyMenu),
		def("delete_menu",        &NLuaAPI::NGUI::LuaGuiDeleteMenu),
		def("remove_menu",        &NLuaAPI::NGUI::LuaGuiRemoveMenu),
		def("check_menu_item",    &NLuaAPI::NGUI::LuaGuiCheckMenuItem),
		def("enable_menu_item",   &NLuaAPI::NGUI::LuaGuiEnableMenuItem),
		def("get_menu_item_id",   &NLuaAPI::NGUI::LuaGuiGetMenuItemId),
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
	object constantTable(from_stack(pState, -1));

#define LUA_AREA_CONSTANT(name) constantTable[#name] = name

	LUA_AREA_CONSTANT(IDOK);
	LUA_AREA_CONSTANT(IDCANCEL);
	LUA_AREA_CONSTANT(IDABORT);
	LUA_AREA_CONSTANT(IDRETRY);
	LUA_AREA_CONSTANT(IDIGNORE);
	LUA_AREA_CONSTANT(IDYES);
	LUA_AREA_CONSTANT(IDNO);
	LUA_AREA_CONSTANT(IDCLOSE);
	LUA_AREA_CONSTANT(IDHELP);
	LUA_AREA_CONSTANT(IDTRYAGAIN);
	LUA_AREA_CONSTANT(IDCONTINUE);
	LUA_AREA_CONSTANT(IDTIMEOUT);
	LUA_AREA_CONSTANT(MB_OK);
	LUA_AREA_CONSTANT(MB_OKCANCEL);
	LUA_AREA_CONSTANT(MB_ABORTRETRYIGNORE);
	LUA_AREA_CONSTANT(MB_YESNOCANCEL);
	LUA_AREA_CONSTANT(MB_YESNO);
	LUA_AREA_CONSTANT(MB_RETRYCANCEL);
	LUA_AREA_CONSTANT(MB_CANCELTRYCONTINUE);
	LUA_AREA_CONSTANT(MB_ICONHAND);
	LUA_AREA_CONSTANT(MB_ICONQUESTION);
	LUA_AREA_CONSTANT(MB_ICONEXCLAMATION);
	LUA_AREA_CONSTANT(MB_ICONASTERISK);
	LUA_AREA_CONSTANT(MB_USERICON);
	LUA_AREA_CONSTANT(MB_ICONWARNING);
	LUA_AREA_CONSTANT(MB_ICONERROR);
	LUA_AREA_CONSTANT(MB_ICONINFORMATION);
	LUA_AREA_CONSTANT(MB_ICONSTOP);
	LUA_AREA_CONSTANT(MB_DEFBUTTON1);
	LUA_AREA_CONSTANT(MB_DEFBUTTON2);
	LUA_AREA_CONSTANT(MB_DEFBUTTON3);
	LUA_AREA_CONSTANT(MB_DEFBUTTON4);
	LUA_AREA_CONSTANT(MB_APPLMODAL);
	LUA_AREA_CONSTANT(MB_SYSTEMMODAL);
	LUA_AREA_CONSTANT(MB_TASKMODAL);
	LUA_AREA_CONSTANT(MB_HELP);
	LUA_AREA_CONSTANT(MB_NOFOCUS);
	LUA_AREA_CONSTANT(MB_SETFOREGROUND);
	LUA_AREA_CONSTANT(MB_DEFAULT_DESKTOP_ONLY);
	LUA_AREA_CONSTANT(MB_TOPMOST);
	LUA_AREA_CONSTANT(MB_RIGHT);
	LUA_AREA_CONSTANT(MB_RTLREADING);
	LUA_AREA_CONSTANT(MB_SERVICE_NOTIFICATION);
	LUA_AREA_CONSTANT(MB_SERVICE_NOTIFICATION_NT3X);
	LUA_AREA_CONSTANT(MB_TYPEMASK);
	LUA_AREA_CONSTANT(MB_ICONMASK);
	LUA_AREA_CONSTANT(MB_DEFMASK);
	LUA_AREA_CONSTANT(MB_MODEMASK);
	LUA_AREA_CONSTANT(MB_MISCMASK);

	LUA_AREA_CONSTANT(WM_NOTIFY);
	LUA_AREA_CONSTANT(WM_INPUTLANGCHANGEREQUEST);
	LUA_AREA_CONSTANT(WM_INPUTLANGCHANGE);
	LUA_AREA_CONSTANT(WM_TCARD);
	LUA_AREA_CONSTANT(WM_HELP);
	LUA_AREA_CONSTANT(WM_USERCHANGED);
	LUA_AREA_CONSTANT(WM_NOTIFYFORMAT);
	LUA_AREA_CONSTANT(WM_CONTEXTMENU);
	LUA_AREA_CONSTANT(WM_STYLECHANGING);
	LUA_AREA_CONSTANT(WM_STYLECHANGED);
	LUA_AREA_CONSTANT(WM_DISPLAYCHANGE);
	LUA_AREA_CONSTANT(WM_GETICON);
	LUA_AREA_CONSTANT(WM_SETICON);
	LUA_AREA_CONSTANT(WM_NCCREATE);
	LUA_AREA_CONSTANT(WM_NCDESTROY);
	LUA_AREA_CONSTANT(WM_NCCALCSIZE);
	LUA_AREA_CONSTANT(WM_NCHITTEST);
	LUA_AREA_CONSTANT(WM_NCPAINT);
	LUA_AREA_CONSTANT(WM_NCACTIVATE);
	LUA_AREA_CONSTANT(WM_GETDLGCODE);
	LUA_AREA_CONSTANT(WM_SYNCPAINT);
	LUA_AREA_CONSTANT(WM_NCMOUSEMOVE);
	LUA_AREA_CONSTANT(WM_NCLBUTTONDOWN);
	LUA_AREA_CONSTANT(WM_NCLBUTTONUP);
	LUA_AREA_CONSTANT(WM_NCLBUTTONDBLCLK);
	LUA_AREA_CONSTANT(WM_NCRBUTTONDOWN);
	LUA_AREA_CONSTANT(WM_NCRBUTTONUP);
	LUA_AREA_CONSTANT(WM_NCRBUTTONDBLCLK);
	LUA_AREA_CONSTANT(WM_NCMBUTTONDOWN);
	LUA_AREA_CONSTANT(WM_NCMBUTTONUP);
	LUA_AREA_CONSTANT(WM_NCMBUTTONDBLCLK);
	LUA_AREA_CONSTANT(WM_NCXBUTTONDOWN);
	LUA_AREA_CONSTANT(WM_NCXBUTTONUP);
	LUA_AREA_CONSTANT(WM_NCXBUTTONDBLCLK);
	LUA_AREA_CONSTANT(WM_INPUT_DEVICE_CHANGE);
	LUA_AREA_CONSTANT(WM_INPUT);
	LUA_AREA_CONSTANT(WM_KEYFIRST);
	LUA_AREA_CONSTANT(WM_KEYDOWN);
	LUA_AREA_CONSTANT(WM_KEYUP);
	LUA_AREA_CONSTANT(WM_CHAR);
	LUA_AREA_CONSTANT(WM_DEADCHAR);
	LUA_AREA_CONSTANT(WM_SYSKEYDOWN);
	LUA_AREA_CONSTANT(WM_SYSKEYUP);
	LUA_AREA_CONSTANT(WM_SYSCHAR);
	LUA_AREA_CONSTANT(WM_SYSDEADCHAR);
	LUA_AREA_CONSTANT(WM_UNICHAR);
	LUA_AREA_CONSTANT(WM_KEYLAST);
	LUA_AREA_CONSTANT(WM_KEYLAST);
	LUA_AREA_CONSTANT(WM_IME_STARTCOMPOSITION);
	LUA_AREA_CONSTANT(WM_IME_ENDCOMPOSITION);
	LUA_AREA_CONSTANT(WM_IME_COMPOSITION);
	LUA_AREA_CONSTANT(WM_IME_KEYLAST);
	LUA_AREA_CONSTANT(WM_INITDIALOG);
	LUA_AREA_CONSTANT(WM_COMMAND);
	LUA_AREA_CONSTANT(WM_SYSCOMMAND);
	LUA_AREA_CONSTANT(WM_TIMER);
	LUA_AREA_CONSTANT(WM_HSCROLL);
	LUA_AREA_CONSTANT(WM_VSCROLL);
	LUA_AREA_CONSTANT(WM_INITMENU);
	LUA_AREA_CONSTANT(WM_INITMENUPOPUP);
	LUA_AREA_CONSTANT(WM_MENUSELECT);
	LUA_AREA_CONSTANT(WM_MENUCHAR);
	LUA_AREA_CONSTANT(WM_ENTERIDLE);
	LUA_AREA_CONSTANT(WM_MENURBUTTONUP);
	LUA_AREA_CONSTANT(WM_MENUDRAG);
	LUA_AREA_CONSTANT(WM_MENUGETOBJECT);
	LUA_AREA_CONSTANT(WM_UNINITMENUPOPUP);
	LUA_AREA_CONSTANT(WM_MENUCOMMAND);
	LUA_AREA_CONSTANT(WM_CHANGEUISTATE);
	LUA_AREA_CONSTANT(WM_UPDATEUISTATE);
	LUA_AREA_CONSTANT(WM_QUERYUISTATE);
	LUA_AREA_CONSTANT(WM_SETTEXT);
	
	LUA_AREA_CONSTANT(MF_INSERT);
	LUA_AREA_CONSTANT(MF_CHANGE);
	LUA_AREA_CONSTANT(MF_APPEND);
	LUA_AREA_CONSTANT(MF_DELETE);
	LUA_AREA_CONSTANT(MF_REMOVE);
	LUA_AREA_CONSTANT(MF_BYCOMMAND);
	LUA_AREA_CONSTANT(MF_BYPOSITION);
	LUA_AREA_CONSTANT(MF_SEPARATOR);
	LUA_AREA_CONSTANT(MF_ENABLED);
	LUA_AREA_CONSTANT(MF_GRAYED);
	LUA_AREA_CONSTANT(MF_DISABLED);
	LUA_AREA_CONSTANT(MF_UNCHECKED);
	LUA_AREA_CONSTANT(MF_CHECKED);
	LUA_AREA_CONSTANT(MF_USECHECKBITMAPS);
	LUA_AREA_CONSTANT(MF_STRING);
	LUA_AREA_CONSTANT(MF_BITMAP);
	LUA_AREA_CONSTANT(MF_OWNERDRAW);
	LUA_AREA_CONSTANT(MF_POPUP);
	LUA_AREA_CONSTANT(MF_MENUBARBREAK);
	LUA_AREA_CONSTANT(MF_MENUBREAK);
	LUA_AREA_CONSTANT(MF_UNHILITE);
	LUA_AREA_CONSTANT(MF_HILITE);
	LUA_AREA_CONSTANT(MF_DEFAULT);
	LUA_AREA_CONSTANT(MF_SYSMENU);
	LUA_AREA_CONSTANT(MF_HELP);
	LUA_AREA_CONSTANT(MF_RIGHTJUSTIFY);
	LUA_AREA_CONSTANT(MF_MOUSESELECT);
	LUA_AREA_CONSTANT(MF_END);
	LUA_AREA_CONSTANT(MFT_STRING);
	LUA_AREA_CONSTANT(MFT_BITMAP);
	LUA_AREA_CONSTANT(MFT_MENUBARBREAK);
	LUA_AREA_CONSTANT(MFT_MENUBREAK);
	LUA_AREA_CONSTANT(MFT_OWNERDRAW);
	LUA_AREA_CONSTANT(MFT_RADIOCHECK);
	LUA_AREA_CONSTANT(MFT_SEPARATOR);
	LUA_AREA_CONSTANT(MFT_RIGHTORDER);
	LUA_AREA_CONSTANT(MFT_RIGHTJUSTIFY);
	LUA_AREA_CONSTANT(MFS_GRAYED);
	LUA_AREA_CONSTANT(MFS_DISABLED);
	LUA_AREA_CONSTANT(MFS_CHECKED);
	LUA_AREA_CONSTANT(MFS_HILITE);
	LUA_AREA_CONSTANT(MFS_ENABLED);
	LUA_AREA_CONSTANT(MFS_UNCHECKED);
	LUA_AREA_CONSTANT(MFS_UNHILITE);
	LUA_AREA_CONSTANT(MFS_DEFAULT);

	return 0;
}
