#pragma once

#include <ydwe/config.h>
#include <cstdarg>
#include <exception>

_BASE_BEGIN 

	class _BASE_API exception : public std::exception
	{
	public:
		exception();
		exception(const char* format, ...);
		exception(const wchar_t* format, ...);
		virtual ~exception();
		virtual const char* what() const;

	protected:
		char*    get_format_string(const char* fmt, va_list argsList) const;
		wchar_t* get_format_string(const wchar_t* fmt, va_list argsList) const;
		char*    what_;
	};

_BASE_END
