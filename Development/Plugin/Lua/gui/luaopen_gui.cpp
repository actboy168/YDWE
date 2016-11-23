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
}

}

int luaopen_gui(lua_State *pState)
{
	using namespace luabind;

	module(pState, "gui")
	[
		def("prompt_for_input",   &NLuaAPI::NGUI::LuaGuiPromptForInput)
	];

	lua_getglobal(pState, "gui");
	return 0;
}
