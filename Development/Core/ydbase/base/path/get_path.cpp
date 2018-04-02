#include <base/path/get_path.h>
#include <base/exception/windows_exception.h>
#include <base/util/dynarray.h>
#include <base/win/env_variable.h>
#include <Windows.h>
#include <assert.h>
#pragma warning(push)
#pragma warning(disable:6387)
#include <Shlobj.h>
#pragma warning(pop)

// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

#define ENSURE(cond) if (FAILED(cond)) throw windows_exception(#cond " failed.");

namespace base { namespace path {

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
		std::wstring result;
		result = win::env_variable(L"TMP").get_nothrow();
		if (!result.empty())
		{
			return std::move(fs::path(result));
		}

		result = win::env_variable(L"TEMP").get_nothrow();
		if (!result.empty())
		{
			return std::move(fs::path(result));
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

	fs::path module(HMODULE module_handle)
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

	fs::path get(PATH_TYPE type)
	{
		wchar_t buffer[MAX_PATH];
		buffer[0] = 0;

		switch (type) 
		{
		case DIR_EXE:
			return std::move(module(NULL));
		case DIR_MODULE:
			return std::move(module(reinterpret_cast<HMODULE>(&__ImageBase)));
		case DIR_TEMP:
			return std::move(temp_path());
		case DIR_WINDOWS:
			return std::move(windows_path());
		case DIR_SYSTEM:
			return std::move(system_path());
		case DIR_PROGRAM_FILES:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_IE_INTERNET_CACHE:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_INTERNET_CACHE, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_COMMON_START_MENU:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_START_MENU:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_APP_DATA:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_COMMON_APP_DATA:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_PROFILE:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_LOCAL_APP_DATA:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_SOURCE_ROOT: 
			{
				fs::path result = get(DIR_EXE);
				result = result.remove_filename();
				return std::move(result);
			}
		case DIR_USER_DESKTOP:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_COMMON_DESKTOP:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_PERSONAL:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_MYPICTURES:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(fs::path(buffer));
		case DIR_USER_QUICK_LAUNCH:
			return std::move(quick_launch_path(false));
		case DIR_DEFAULT_USER_QUICK_LAUNCH:
			return std::move(quick_launch_path(true));
		case DIR_TASKBAR_PINS:
			{
				fs::path result = get(DIR_USER_QUICK_LAUNCH);
				result = result / L"User Pinned" / L"TaskBar";
				return std::move(result);
			}
		default:
			assert(false);
			return std::move(fs::path());
		}
	}
}}
