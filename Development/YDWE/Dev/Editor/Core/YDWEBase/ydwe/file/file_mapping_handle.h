#pragma once

#include <ydwe/win/scoped_handle.h>
#include <ydwe/file/file_handle.h>
#include <Windows.h>

_BASE_BEGIN namespace file {

	class file_mapping_handle : public win::scoped_handle<NULL>
	{
		typedef win::scoped_handle<NULL> _Mybase;

	public:
		file_mapping_handle(file_handle const& file, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, uint64_t dwMaximumSize, LPCWSTR lpName);
		void* create_view(DWORD dwDesiredAccess, uint64_t dwFileOffset, size_t dwNumberOfBytesToMap);
	};
}}
