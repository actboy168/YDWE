#pragma once

#if !defined(_M_X64)
#include <base/config.h>
#include <string>
#include <Windows.h>

namespace base { namespace hook {
	_BASE_API bool openprocess(DWORD pid, DWORD process_access, DWORD thread_access, PROCESS_INFORMATION& pi);
	_BASE_API bool injectdll(const PROCESS_INFORMATION& pi, const std::wstring& x86dll, const std::wstring& x64dll);
	_BASE_API bool injectdll(DWORD pid, const std::wstring& x86dll, const std::wstring& x64dll);
}}
#endif
