
#include <base/path/detail/get_path.h>
#include <base/path/service.h>
#include <base/exception/windows_exception.h>
#include <base/util/dynarray.h>
#include <base/win/env_variable.h>
#include <Psapi.h>
#pragma warning(push)
#pragma warning(disable:6387)
#include <Shlobj.h>
#pragma warning(pop)
#pragma comment(lib, "Psapi.lib")

#define ENSURE(cond) if (FAILED(cond)) throw windows_exception(#cond " failed.");

namespace base { namespace path { namespace detail {
	fs::path quick_launch_path(bool default_user) 
	{
		if (default_user) 
		{
			wchar_t buffer[MAX_PATH];
			buffer[0] = 0;
			// http://msdn.microsoft.com/library/windows/desktop/bb762181.aspx
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_APPDATA, reinterpret_cast<HANDLE>(-1), SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		} 
		else
		{
			fs::path result = get(DIR_APP_DATA);
			// http://stackoverflow.com/questions/76080/how-do-you-reliably-get-the-quick-
			// http://www.microsoft.com/technet/scriptcenter/resources/qanda/sept05/hey0901.mspx
			result = result / L"Microsoft" / L"Internet Explorer" / L"Quick Launch";
			return std::move(result);
		}
	}

	// 
	// https://blogs.msdn.com/b/larryosterman/archive/2010/10/19/because-if-you-do_2c00_-stuff-doesn_2700_t-work-the-way-you-intended_2e00_.aspx
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa364992%28v=vs.85%29.aspx
	//
	fs::path temp_path() 
	{
		std::optional<std::wstring> result;
		result = win::env_variable(L"TMP").get_nothrow();
		if (result && !result->empty())
		{
			return std::move(fs::path(result.get()));
		}

		result = win::env_variable(L"TEMP").get_nothrow();
		if (result && !result->empty())
		{
			return std::move(fs::path(result.get()));
		}

		std::dynarray<wchar_t> buffer(::GetTempPathW(0, nullptr));
		if (buffer.empty() || ::GetTempPathW(buffer.size(), &buffer[0]) == 0)
		{
			throw windows_exception("::GetTempPathW failed.");
		}

		fs::path p(buffer.begin(), buffer.begin() + buffer.size() - 1);
		if (!fs::is_directory(p))
		{
			throw windows_exception("::GetTempPathW failed.");
		}
		return std::move(p);
	}

	fs::path windows_path()
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetWindowsDirectoryW(buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw windows_exception("::GetWindowsDirectoryW failed.");
		}

		if (path_len <= _countof(buffer))
		{
			return std::move(fs::path(buffer, buffer + path_len));
		}

		std::dynarray<wchar_t> buf(path_len);
		path_len = ::GetWindowsDirectoryW(buf.data(), buf.size());
		if (path_len == 0 || path_len > _countof(buffer))
		{
			throw windows_exception("::GetWindowsDirectoryW failed.");
		}
		return std::move(fs::path(buf.begin(), buf.end()));
	}

	fs::path system_path()
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetSystemDirectoryW(buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw windows_exception("::GetSystemDirectoryW failed.");
		}

		if (path_len <= _countof(buffer))
		{
			return std::move(fs::path(buffer, buffer + path_len));
		}

		std::dynarray<wchar_t> buf(path_len);
		path_len = ::GetWindowsDirectoryW(buf.data(), buf.size());
		if (path_len == 0 || path_len > _countof(buffer))
		{
			throw windows_exception("::GetSystemDirectoryW failed.");
		}
		return std::move(fs::path(buf.begin(), buf.end()));
	}

	fs::path module_path(HMODULE module_handle)
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetModuleFileNameW(module_handle, buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw windows_exception("::GetModuleFileNameW failed.");
		}

		if (path_len < _countof(buffer))
		{
			return std::move(fs::path(buffer, buffer + path_len));
		}

		for (size_t buf_len = 0x200; buf_len <= 0x10000; buf_len <<= 1)
		{
			std::dynarray<wchar_t> buf(path_len);
			path_len = ::GetModuleFileNameW(module_handle, buf.data(), buf.size());		
			if (path_len == 0)
			{
				throw windows_exception("::GetModuleFileNameW failed.");
			}

			if (path_len < _countof(buffer))
			{
				return std::move(fs::path(buf.begin(), buf.end()));
			}
		}

		throw windows_exception("::GetModuleFileNameW failed.");
	}

	fs::path module_path(HANDLE process_handle, HMODULE module_handle)
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetModuleFileNameExW(process_handle, module_handle, buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw windows_exception("::GetModuleFileNameExW failed.");
		}

		if (path_len < _countof(buffer))
		{
			return std::move(fs::path(buffer, buffer + path_len));
		}

		for (size_t buf_len = 0x200; buf_len <= 0x10000; buf_len <<= 1)
		{
			std::dynarray<wchar_t> buf(path_len);
			path_len = ::GetModuleFileNameExW(process_handle, module_handle, buf.data(), buf.size());
			if (path_len == 0)
			{
				throw windows_exception("::GetModuleFileNameExW failed.");
			}

			if (path_len < _countof(buffer))
			{
				return std::move(fs::path(buf.begin(), buf.end()));
			}
		}

		throw windows_exception("::GetModuleFileNameExW failed.");
	}
}}}
