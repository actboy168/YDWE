#pragma once

#include <Windows.h>	

namespace base { namespace hook {
	BOOL WINAPI ReplaceImport(HANDLE hProcess, LPCSTR lpOldDll, LPCSTR lpNewDll);
}}
