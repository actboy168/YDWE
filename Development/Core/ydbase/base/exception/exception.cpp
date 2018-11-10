#include <base/exception/exception.h>
#include <bee/utility/unicode.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

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
		werrormsg what = get_format_string(fmt, YD_VA_START(fmt));
		if (what) {
			std::string temp_string = bee::w2u(what.c_str());
			if (what_) {
				what_.reset(temp_string.c_str(), temp_string.size() + 1);
			}
		}
	}

	exception::~exception()
	{ }

	exception::errormsg exception::get_format_string(const char* fmt, ...) const {
		return get_format_string(fmt,  YD_VA_START(fmt));
	}

	exception::errormsg exception::get_format_string(const char* fmt, va_list argsList) const {
		size_t buffer_size = ::_vscprintf(fmt, argsList) + 1;
		errormsg buffer(buffer_size);
		if (buffer) {
			int n = ::_vsnprintf_s(buffer.c_str(), buffer_size, buffer_size-1, fmt, argsList);
			if (n > 0) {
				return buffer;
			}
		}
		return errormsg();
	}

	exception::werrormsg exception::get_format_string(const wchar_t* fmt, ...) const {
		return get_format_string(fmt,  YD_VA_START(fmt));
	}

	exception::werrormsg exception::get_format_string(const wchar_t* fmt, va_list argsList) const {
		size_t buffer_size = ::_vscwprintf(fmt, argsList) + 1;
		werrormsg buffer(buffer_size);
		if (buffer) {
			int n = ::_vsnwprintf_s(buffer.c_str(), buffer_size, buffer_size-1, fmt, argsList);
			if (n > 0) {
				return buffer;
			}
		}
		return werrormsg();
	}

	const char* exception::what() const {
		return what_ ? what_.c_str() : "unknown base::exception";
	}

#undef YD_VA_START
}
