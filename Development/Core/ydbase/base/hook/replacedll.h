#pragma once

#include <base/config.h>
#include <Windows.h>	

namespace base { namespace hook {
	_BASE_API bool replacedll(const PROCESS_INFORMATION& pi, const char* oldDll, const char* newDll);
}}
