#include <base/file/file_mapping_handle.h>
#include <base/exception/windows_exception.h>

namespace base { namespace file {

	file_mapping_handle::file_mapping_handle(file_handle const& file, LPSECURITY_ATTRIBUTES lpAttributes, uint32_t flProtect, uint64_t dwMaximumSize, const wchar_t* name)
		: _Mybase(::CreateFileMappingW(file.get(), lpAttributes, flProtect, static_cast<DWORD>(dwMaximumSize >> 32), static_cast<DWORD>(dwMaximumSize), name))
	{
		if (!_Mybase::operator bool())
		{
			throw windows_exception("failed to open file mapping");
		}
	}

	file_mapping_handle::file_mapping_handle(uint32_t dwDesiredAccess, bool bInheritHandle, const wchar_t* name)
		: _Mybase(::OpenFileMappingW(dwDesiredAccess, bInheritHandle, name))
	{
		if (!_Mybase::operator bool())
		{
			throw windows_exception("failed to open file mapping");
		}
	}

	void* file_mapping_handle::create_view(DWORD dwDesiredAccess, uint64_t dwFileOffset, size_t dwNumberOfBytesToMap)
	{
		void* memory_view =  ::MapViewOfFile(_Mybase::get(), dwDesiredAccess, static_cast<DWORD>(dwFileOffset >> 32), static_cast<DWORD>(dwFileOffset), dwNumberOfBytesToMap);

		if (nullptr == memory_view)
		{
			throw windows_exception("failed to map view of file");
		}

		return memory_view;
	}
}}
