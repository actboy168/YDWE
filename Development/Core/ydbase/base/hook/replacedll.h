#pragma once

#include <Windows.h>	

namespace base { namespace hook {
	bool replacedll(HANDLE hProcess, const char* oldDll, const char* newDll);
}}
