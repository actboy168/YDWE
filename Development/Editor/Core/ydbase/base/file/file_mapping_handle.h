#pragma once

#include <base/win/scoped_handle.h>
#include <base/file/file_handle.h>
#include <Windows.h>

namespace base { namespace file {

	class file_mapping_handle 
		: public win::scoped_handle<NULL>
	{
		typedef win::scoped_handle<NULL> mybase;

	public:
		file_mapping_handle(HANDLE file, LPSECURITY_ATTRIBUTES lpAttributes, uint32_t flProtect, uint64_t dwMaximumSize, const wchar_t* name);
		file_mapping_handle(uint32_t dwDesiredAccess, bool bInheritHandle, const wchar_t* name);
		void* create_view(DWORD dwDesiredAccess, uint64_t dwFileOffset, size_t dwNumberOfBytesToMap);
	};
}}
