#pragma once

#include <Windows.h>	

namespace base { namespace hook {
	bool replace_import(HANDLE hProcess, LPCSTR lpOldDll, LPCSTR lpNewDll);
}}
