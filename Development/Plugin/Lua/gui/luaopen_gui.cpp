#pragma warning(push, 3)
#include <lua.hpp>
#pragma warning(pop)
#include <base/exception/windows_exception.h>  
#include <base/util/unicode.h>
#include <tuple>
#include <Windows.h>
#include <cstdint>
#include "resource.h"

extern HMODULE self_module;

std::wstring luaL_checkwstring(lua_State* L, int index)
{
	size_t len = 0;
	const char* str = luaL_checklstring(L, index, &len);
	return base::u2w(std::string_view(str, len), base::conv_method::replace | '?');
}

void lua_pushwstring(lua_State* L, const std::wstring& str)
{
	std::string ustr = base::w2u(str, base::conv_method::replace | '?');
	lua_pushlstring(L, ustr.data(), ustr.size());
}

typedef std::tuple<lua_State *, std::wstring, std::wstring, std::wstring, std::wstring, std::wstring> TInputDialogInitInfo;

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
		TInputDialogInitInfo* pInfo = reinterpret_cast<TInputDialogInitInfo *>(lParam);

		SetWindowTextW(dialogHandle, std::get<1>(*pInfo).c_str());
		SetDlgItemTextW(dialogHandle, IDC_PROMPT, std::get<2>(*pInfo).c_str());
		SetDlgItemTextW(dialogHandle, IDC_EDIT_CONTENT, std::get<3>(*pInfo).c_str());
		SetDlgItemTextW(dialogHandle, IDOK, std::get<4>(*pInfo).c_str());
		SetDlgItemTextW(dialogHandle, IDCANCEL, std::get<5>(*pInfo).c_str());
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

static int LuaGuiPromptForInput(lua_State* L)
{
	HWND parentWindowHandle = (HWND)luaL_checkinteger(L, 1);
	std::wstring caption = luaL_checkwstring(L, 2);
	std::wstring prompt = luaL_checkwstring(L, 3);
	std::wstring initialText = luaL_checkwstring(L, 4);
	std::wstring okCaption = luaL_checkwstring(L, 5);
	std::wstring cancelCaption = luaL_checkwstring(L, 6);

	TInputDialogInitInfo dialogInitInfo(L, caption, prompt, initialText, okCaption, cancelCaption);

	SInputInfo *pInputInfo = reinterpret_cast<SInputInfo *>(::DialogBoxParamW(self_module, MAKEINTRESOURCEW(IDD_DIALOG_INPUT), parentWindowHandle, PromptForInputDialogProc, reinterpret_cast<LPARAM>(&dialogInitInfo)));

	if ((intptr_t)pInputInfo == -1)
	{
		base::windows_exception e;
		return luaL_error(L, e.what());
	}
	lua_pushboolean(L, pInputInfo->isOkClicked);
	lua_pushwstring(L, std::wstring(pInputInfo->text));
	free(pInputInfo);
	return 2;
}

int luaopen_gui(lua_State* L)
{
	luaL_Reg l[] = {
		{ "prompt_for_input", LuaGuiPromptForInput },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	lua_setglobal(L, "gui");
	return 0;
}
