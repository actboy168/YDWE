#pragma once

#include <ydwe/config.h>
#include <cstdarg>
#include <exception>

namespace ydwe {
	class _BASE_API exception : public std::exception
	{
	public:
		exception();
		exception(const wchar_t* format, ...);
		virtual ~exception();
		virtual const char* what() const;
		virtual const wchar_t* c_str() const;

	protected:
		wchar_t* get_format_string(const wchar_t* fmt, va_list argsList) const;
		wchar_t* what_;
	};
}
