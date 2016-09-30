#pragma once

#include <Windows.h>

namespace windows7
{
	HRESULT SetCurrentProcessExplicitAppUserModelID(PCWSTR AppID);
	HRESULT GetCurrentProcessExplicitAppUserModelID(PWSTR *AppID);
	HRESULT ShellCreateItemFromParsingName(PCWSTR pszPath, IBindCtx *pbc, const IID* riid, void **ppv);
}
