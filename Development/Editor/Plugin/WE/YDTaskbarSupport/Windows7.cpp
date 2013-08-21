#include "Windows7.h"
#include <aero/function/fp_call.hpp>

namespace windows7
{
	HRESULT SetCurrentProcessExplicitAppUserModelID(PCWSTR AppID)
	{
		static void* pSetCurrentProcessExplicitAppUserModelID = ::GetProcAddress(::LoadLibraryW(L"Shell32.dll"), "SetCurrentProcessExplicitAppUserModelID");

		if (pSetCurrentProcessExplicitAppUserModelID == NULL)
		{
			return E_FAIL;
		}

		return aero::std_call<HRESULT>(pSetCurrentProcessExplicitAppUserModelID, AppID);
	}

	HRESULT ShellCreateItemFromParsingName(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv)
	{
		static void* pSHCreateItemFromParsingName = ::GetProcAddress(::LoadLibraryW(L"Shell32.dll"), "SHCreateItemFromParsingName");

		if (pSHCreateItemFromParsingName == NULL)
		{
			return E_FAIL;
		}

		return aero::std_call<HRESULT>(pSHCreateItemFromParsingName, pszPath, pbc, riid, ppv);
	}		
}
