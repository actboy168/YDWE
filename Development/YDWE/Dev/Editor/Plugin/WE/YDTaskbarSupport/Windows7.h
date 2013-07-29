#pragma once

#include <Windows.h>

namespace windows7
{
	HRESULT SetCurrentProcessExplicitAppUserModelID(PCWSTR AppID);
	HRESULT ShellCreateItemFromParsingName(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv);
}
