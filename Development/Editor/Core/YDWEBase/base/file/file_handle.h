#pragma once

#include <base/win/scoped_handle.h>
#include <Windows.h>
#include <cstdint>

namespace base { namespace file {

	class file_handle 
		: public win::scoped_handle<INVALID_HANDLE_VALUE>
	{
		typedef win::scoped_handle<INVALID_HANDLE_VALUE> mybase;

	public:
		file_handle(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
		uint64_t get_size();
	};
}}
