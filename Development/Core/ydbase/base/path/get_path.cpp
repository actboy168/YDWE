#include <base/path/get_path.h>
#include <bee/exception/windows_exception.h>
#include <bee/utility/dynarray.h>
#include <base/win/env_variable.h>
#include <Windows.h>

namespace base { namespace path {

	// 
	// https://blogs.msdn.com/b/larryosterman/archive/2010/10/19/because-if-you-do_2c00_-stuff-doesn_2700_t-work-the-way-you-intended_2e00_.aspx
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa364992%28v=vs.85%29.aspx
	//
	fs::path temp()
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
			throw bee::windows_exception("::GetTempPathW failed.");
		}

		fs::path p(buffer.begin(), buffer.begin() + buffer.size() - 1);
		if (!fs::is_directory(p))
		{
			throw bee::windows_exception("::GetTempPathW failed.");
		}
		return std::move(p);
	}

	fs::path module(HMODULE module_handle)
	{
		wchar_t buffer[MAX_PATH];
		DWORD path_len = ::GetModuleFileNameW(module_handle, buffer, _countof(buffer));
		if (path_len == 0)
		{
			throw bee::windows_exception("::GetModuleFileNameW failed.");
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
				throw bee::windows_exception("::GetModuleFileNameW failed.");
			}

			if (path_len < _countof(buffer))
			{
				return std::move(fs::path(buf.begin(), buf.end()));
			}
		}

		throw bee::windows_exception("::GetModuleFileNameW failed.");
	}
}}
