#include <base/exception/exception.h>
#include <base/util/unicode.h>
#include <Windows.h>
#include <cstdio>
#include <cassert>
#include <cstdint>

namespace base { 

#define YD_VA_START(v)  ((va_list)_ADDRESSOF(v) + _INTSIZEOF(v))

	exception::exception()
		: what_()
	{ }

	exception::exception(const char* fmt, ...)
		: what_(get_format_string(fmt, YD_VA_START(fmt)))
	{ }

	exception::exception(const wchar_t* fmt, ...)
		: what_()
	{
		error_wmsg what = get_format_string(fmt, YD_VA_START(fmt));
		if (what)
		{
			std::string temp_string = w2u(what.c_str(), conv_method::replace | '?');

			what_.alloc(temp_string.size() + 1);
			if (what_)
			{
				what_.copy_str(temp_string.c_str(), temp_string.size() + 1);
			}
		}
	}

	exception::~exception()
	{ }

	exception::error_msg exception::get_format_string(const char* fmt, ...) const
	{
		return get_format_string(fmt,  YD_VA_START(fmt));
	}

	exception::error_msg exception::get_format_string(const char* fmt, va_list argsList) const
	{
		size_t buffer_size = ::_vscprintf(fmt, argsList) + 1;

		error_msg buffer(buffer_size);

		if (buffer)
		{
			int n = ::_vsnprintf_s(buffer.c_str(), buffer_size, buffer_size-1, fmt, argsList);
			if (n > 0)
			{
				return buffer;
			}
		}

		return error_msg();
	}

	exception::error_wmsg exception::get_format_string(const wchar_t* fmt, ...) const
	{
		return get_format_string(fmt,  YD_VA_START(fmt));
	}

	exception::error_wmsg exception::get_format_string(const wchar_t* fmt, va_list argsList) const
	{
		size_t buffer_size = ::_vscwprintf(fmt, argsList) + 1;

		error_wmsg buffer(buffer_size);

		if (buffer)
		{
			int n = ::_vsnwprintf_s(buffer.c_str(), buffer_size, buffer_size-1, fmt, argsList);
			if (n > 0)
			{
				return buffer;
			}
		}

		return error_wmsg();
	}

	const char* exception::what() const
	{
		return what_ ? what_.c_str() : "unknown base::exception";
	}

#undef YD_VA_START

}
