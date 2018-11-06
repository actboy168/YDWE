#include <base/file/file_handle.h>
#include <base/exception/windows_exception.h>

namespace base { namespace file {

	file_handle::file_handle(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
		: mybase(::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile))
	{
		if (!mybase::operator bool())
		{
			throw windows_exception("failed to open file");
		}
	}

	uint64_t file_handle::get_size()
	{
		LARGE_INTEGER file_size;
		if (!::GetFileSizeEx(mybase::get(), &file_size)) {
			throw windows_exception("failed to determine file size");
		}
		return file_size.QuadPart;
	}
}}
