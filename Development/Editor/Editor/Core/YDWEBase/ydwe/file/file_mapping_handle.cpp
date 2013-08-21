#include <ydwe/file/file_mapping_handle.h>
#include <ydwe/exception/windows_exception.h>

namespace ydwe { namespace file {

	file_mapping_handle::file_mapping_handle(file_handle const& file, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, uint64_t dwMaximumSize, LPCWSTR lpName)
		: _Mybase(::CreateFileMappingW(file.get(), lpAttributes, flProtect, static_cast<DWORD>(dwMaximumSize >> 32), static_cast<DWORD>(dwMaximumSize), lpName))
	{
		if (!_Mybase::operator bool())
		{
			throw windows_exception(L"failed to open file mapping");
		}
	}

	void* file_mapping_handle::create_view(DWORD dwDesiredAccess, uint64_t dwFileOffset, size_t dwNumberOfBytesToMap)
	{
		void* memory_view =  ::MapViewOfFile(_Mybase::get(), dwDesiredAccess, static_cast<DWORD>(dwFileOffset >> 32), static_cast<DWORD>(dwFileOffset), dwNumberOfBytesToMap);

		if (nullptr == memory_view)
		{
			throw windows_exception(L"failed to map view of file");
		}

		return memory_view;
	}
}}
