#include "Windows7.h"
#include <base/hook/fp_call.h>

namespace windows7
{
	HRESULT SetCurrentProcessExplicitAppUserModelID(PCWSTR AppID)
	{
		static void* pSetCurrentProcessExplicitAppUserModelID = ::GetProcAddress(::LoadLibraryW(L"Shell32.dll"), "SetCurrentProcessExplicitAppUserModelID");

		if (pSetCurrentProcessExplicitAppUserModelID == NULL)
		{
			return E_FAIL;
		}

		return base::std_call<HRESULT>(pSetCurrentProcessExplicitAppUserModelID, AppID);
	}

	HRESULT GetCurrentProcessExplicitAppUserModelID(PWSTR *AppID)
	{
		static void* pGetCurrentProcessExplicitAppUserModelID = ::GetProcAddress(::LoadLibraryW(L"Shell32.dll"), "GetCurrentProcessExplicitAppUserModelID");

		if (pGetCurrentProcessExplicitAppUserModelID == NULL)
		{
			return E_FAIL;
		}

		return base::std_call<HRESULT>(pGetCurrentProcessExplicitAppUserModelID, AppID);
	}
	
	HRESULT ShellCreateItemFromParsingName(PCWSTR pszPath, IBindCtx *pbc, const IID* riid, void **ppv)
	{
		static void* pSHCreateItemFromParsingName = ::GetProcAddress(::LoadLibraryW(L"Shell32.dll"), "SHCreateItemFromParsingName");

		if (pSHCreateItemFromParsingName == NULL)
		{
			return E_FAIL;
		}

		return base::std_call<HRESULT>(pSHCreateItemFromParsingName, pszPath, pbc, riid, ppv);
	}		
}
