#include <ydwe/exception/exception.h>
#include <ydwe/util/unicode.h>
#include <Windows.h>
#include <cstdio>
#include <cassert>
#include <cstdint>

_BASE_BEGIN 

	exception::exception()
		: what_(nullptr)
	{ }

	exception::exception(const char* format, ...)
		: what_(nullptr)
	{
		va_list ap;
		va_start(ap, format);
		what_ = get_format_string(format, ap);
		va_end(ap);
	}

	exception::exception(const wchar_t* format, ...)
		: what_(nullptr)
	{
		va_list ap;
		va_start(ap, format);
		wchar_t* what = get_format_string(format, ap);
		if (what)
		{
			static std::string temp_string;
			temp_string = util::w2u(what, util::conv_method::replace | '?');
			::LocalFree(reinterpret_cast<HLOCAL>(what));

			char* buffer = static_cast<char*>(::LocalAlloc(0, (temp_string.size() + 1) * sizeof (char)));
			if (buffer)
			{
				strcpy_s(buffer, temp_string.size() + 1, temp_string.c_str());
				what_ = buffer;
			}
		}
		va_end(ap);
	}

	exception::~exception()
	{
		if (what_)
		{
			::LocalFree(reinterpret_cast<HLOCAL>(what_));
			what_ = nullptr;
		}
	}

	char* exception::get_format_string(const char* fmt, ...) const
	{
		char* result = nullptr;
		va_list ap;
		va_start(ap, fmt);
		result = get_format_string(fmt, ap);
		va_end(ap);

		return result;
	}

	char* exception::get_format_string(const char* fmt, va_list argsList) const
	{
		size_t buffer_size = ::_vscprintf(fmt, argsList) + 1;

		char* buffer = static_cast<char*>(::LocalAlloc(0, buffer_size * sizeof (char)));

		if (buffer)
		{
			int n = ::_vsnprintf_s(buffer, buffer_size, buffer_size, fmt, argsList);
			if (n > 0)
			{
				return buffer;
			}
		}

		return nullptr;
	}

	wchar_t* exception::get_format_string(const wchar_t* fmt, ...) const
	{
		wchar_t* result = nullptr;
		va_list ap;
		va_start(ap, fmt);
		result = get_format_string(fmt, ap);
		va_end(ap);

		return result;
	}

	wchar_t* exception::get_format_string(const wchar_t* fmt, va_list argsList) const
	{
		size_t buffer_size = ::_vscwprintf(fmt, argsList) + 1;

		wchar_t* buffer = static_cast<wchar_t*>(::LocalAlloc(0, buffer_size * sizeof (wchar_t)));

		if (buffer)
		{
			int n = ::_vsnwprintf_s(buffer, buffer_size, buffer_size, fmt, argsList);
			if (n > 0)
			{
				return buffer;
			}
		}

		return nullptr;
	}

	const char* exception::what() const
	{
		return what_ ? what_ : "unknown ydwe::exception";
	}

_BASE_END
