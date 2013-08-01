
#include <ydwe/path/detail/get_path.h>
#include <ydwe/path/service.h>
#include <Shlobj.h>
#include <ydwe/exception/windows_exception.h>

#define ENSURE(cond) if (FAILED(cond)) throw windows_exception(L ## #cond L" failed.");

_BASE_BEGIN namespace path { namespace detail {
	boost::filesystem::path GetQuickLaunchPath(bool default_user) 
	{
		if (default_user) 
		{
			wchar_t buffer[MAX_PATH];
			buffer[0] = 0;
			// http://msdn.microsoft.com/library/windows/desktop/bb762181.aspx
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_APPDATA, reinterpret_cast<HANDLE>(-1), SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		} 
		else
		{
			boost::filesystem::path result = get(DIR_APP_DATA);
			// http://stackoverflow.com/questions/76080/how-do-you-reliably-get-the-quick-
			// http://www.microsoft.com/technet/scriptcenter/resources/qanda/sept05/hey0901.mspx
			result = result / L"Microsoft" / L"Internet Explorer" / L"Quick Launch";
			return std::move(result);
		}
	}

	boost::filesystem::path GetTempPath() 
	{
		wchar_t buffer[MAX_PATH + 1];
		DWORD path_len = ::GetTempPathW(MAX_PATH, buffer);
		if (path_len >= MAX_PATH || path_len <= 0)
			throw windows_exception(L"::GetTempPathW(MAX_PATH, buffer) failed.");

		return std::move(boost::filesystem::path(buffer));
	}

	boost::filesystem::path GetWindowsPath() 
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetWindowsDirectoryW(buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw windows_exception(L"::GetWindowsDirectoryW failed.");
		}

		if (path_len <= _countof(buffer))
		{
			return std::move(boost::filesystem::path(buffer, buffer + path_len));
		}

		std::unique_ptr<wchar_t[]> buf(new wchar_t[path_len]);
		path_len = ::GetWindowsDirectoryW(buf.get(), path_len);
		if (path_len == 0 || path_len > _countof(buffer))
		{
			throw windows_exception(L"::GetWindowsDirectoryW failed.");
		}
		return std::move(boost::filesystem::path(buf.get(), buf.get() + path_len));
	}

	boost::filesystem::path GetSystemPath() 
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetSystemDirectoryW(buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw windows_exception(L"::GetSystemDirectoryW failed.");
		}

		if (path_len <= _countof(buffer))
		{
			return std::move(boost::filesystem::path(buffer, buffer + path_len));
		}

		std::unique_ptr<wchar_t[]> buf(new wchar_t[path_len]);
		path_len = ::GetWindowsDirectoryW(buf.get(), path_len);
		if (path_len == 0 || path_len > _countof(buffer))
		{
			throw windows_exception(L"::GetSystemDirectoryW failed.");
		}
		return std::move(boost::filesystem::path(buf.get(), buf.get() + path_len));
	}

	boost::filesystem::path GetModulePath(HMODULE module_handle)
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetModuleFileNameW(module_handle, buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw windows_exception(L"::GetModuleFileNameW failed.");
		}

		if (path_len < _countof(buffer))
		{
			return std::move(boost::filesystem::path(buffer, buffer + path_len));
		}

		for (size_t buf_len = 0x200; buf_len <= 0x10000; buf_len <<= 1)
		{
			std::unique_ptr<wchar_t[]> buf(new wchar_t[buf_len]);
			DWORD path_len = ::GetModuleFileNameW(module_handle, buf.get(), buf_len);		
			if (path_len == 0)
			{
				throw windows_exception(L"::GetModuleFileNameW failed.");
			}

			if (path_len < _countof(buffer))
			{
				return std::move(boost::filesystem::path(buf.get(), buf.get() + path_len));
			}
		}

		throw windows_exception(L"::GetModuleFileNameW failed.");
	}
}}}
