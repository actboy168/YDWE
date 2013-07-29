#include <ydwe/exception/exception.h>
#include <ydwe/util/unicode.h>
#include <Windows.h>
#include <cstdio>
#include <cassert>
#include <cstdint>

namespace ydwe {

	exception::exception()
		: what_(nullptr)
	{ }

	exception::exception(const wchar_t* format, ...)
		: what_(nullptr)
	{
		va_list ap;
		va_start(ap, format);
		what_ = get_format_string(format, ap);
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

	wchar_t* exception::get_format_string(const wchar_t* fmt, va_list argsList) const
	{
		size_t buffer_size = ::_vscwprintf(fmt, argsList) + 1;

		wchar_t* buffer = static_cast<wchar_t*>(::LocalAlloc(0, buffer_size * sizeof (wchar_t)));

		if (buffer)
		{
#ifdef _MSC_VER
			int n = ::_vsnwprintf_s(buffer, buffer_size, buffer_size, fmt, argsList);
#else
			int n = ::_vsnwprintf(buffer, buffer_size, fmt, argsList);
#endif
			if (n > 0)
			{
				return buffer;
			}
		}

		return nullptr;
	}

	const char* exception::what() const
	{
		if (!what_)
		{
			return "unknown ydwe::exception";
		}

		static std::string temp_string;
		temp_string = util::w2u(what_);
		return temp_string.c_str();
	}

	const wchar_t* exception::c_str() const
	{
		return what_ ? what_ : L"unknown ydwe::exception";
	}
}
