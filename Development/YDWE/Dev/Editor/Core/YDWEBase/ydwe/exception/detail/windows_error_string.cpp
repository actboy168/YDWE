#include <ydwe/exception/detail/windows_error_string.h>
#include <Windows.h>
#include <cstdio>

_BASE_BEGIN namespace exception_detail {

	const wchar_t* windows_error_string::default_error_string_fmt = L"error code(%08X)";

	wchar_t* windows_error_string::create(uint32_t error_code)
	{
		wchar_t* str = nullptr;

		if (0 != system_error_string(error_code, &str))
		{
			return str;
		}

		if (0 != default_error_string(error_code, &str))
		{
			return str;
		}

		return L"out of memory";
	}

	void windows_error_string::release(wchar_t* str)
	{
		if (str)
		{
			::LocalFree(reinterpret_cast<HLOCAL>(str));
		}
	}

	uint32_t windows_error_string::system_error_string(uint32_t error_code, wchar_t** buffer_ptr)
	{
		return ::FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			error_code, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			reinterpret_cast<LPWSTR>(buffer_ptr), 
			0, 
			NULL);
	}

	uint32_t windows_error_string::default_error_string(uint32_t error_code, wchar_t** buffer_ptr)
	{
		if (!buffer_ptr)
		{
			return 0;
		}

		uint32_t buffer_size = ::_scwprintf(default_error_string_fmt, error_code) + 1;

		wchar_t* buffer = static_cast<wchar_t*>(::LocalAlloc(0, buffer_size * sizeof(wchar_t)));

		if (!buffer)
		{
			return 0;
		}
#ifdef _MSC_VER
		int n = ::_snwprintf_s(buffer, buffer_size, buffer_size, default_error_string_fmt, error_code);
#else
		int n = ::_snwprintf(buffer, buffer_size, default_error_string_fmt, error_code);
#endif
		if (n <= 0)
		{
			return 0;
		}
		*buffer_ptr = buffer;
		return n;
	}
}}
