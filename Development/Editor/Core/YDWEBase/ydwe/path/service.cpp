#include <ydwe/path/service.h>

#include <windows.h>
#include <ydwe/exception/windows_exception.h>
#include <ydwe/path/detail/get_path.h>
#pragma warning(push)
#pragma warning(disable:6387)
#include <Shlobj.h>
#pragma warning(pop)

// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

#define ENSURE(cond) if (FAILED(cond)) throw windows_exception(#cond " failed.");

_BASE_BEGIN namespace path {

	boost::filesystem::path get(PATH_TYPE type)
	{
		wchar_t buffer[MAX_PATH];
		buffer[0] = 0;

		switch (type) 
		{
		case DIR_EXE:
			return std::move(detail::GetModulePath(NULL));
		case DIR_MODULE:
			return std::move(detail::GetModulePath(reinterpret_cast<HMODULE>(&__ImageBase)));
		case DIR_TEMP:
			return std::move(detail::GetTempPath());
		case DIR_WINDOWS:
			return std::move(detail::GetWindowsPath());
		case DIR_SYSTEM:
			return std::move(detail::GetSystemPath());
		case DIR_PROGRAM_FILES:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_IE_INTERNET_CACHE:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_INTERNET_CACHE, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_COMMON_START_MENU:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_START_MENU:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_APP_DATA:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_COMMON_APP_DATA:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_PROFILE:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_LOCAL_APP_DATA:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_SOURCE_ROOT: 
			{
				boost::filesystem::path result = get(DIR_EXE);
				result = result.remove_filename();
				return std::move(result);
			}
		case DIR_USER_DESKTOP:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_COMMON_DESKTOP:
			ENSURE(::SHGetFolderPathW(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, buffer));
			return std::move(boost::filesystem::path(buffer));
		case DIR_USER_QUICK_LAUNCH:
			return std::move(detail::GetQuickLaunchPath(false));
		case DIR_DEFAULT_USER_QUICK_LAUNCH:
			return std::move(detail::GetQuickLaunchPath(true));
		case DIR_TASKBAR_PINS:
			{
				boost::filesystem::path result = get(DIR_USER_QUICK_LAUNCH);
				result = result / L"User Pinned" / L"TaskBar";
				return std::move(result);
			}
		default:
			assert(false);
			return std::move(boost::filesystem::path());
		}
	}
}}
