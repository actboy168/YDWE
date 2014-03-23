#pragma once

#include <base/config.h>
#include <base/exception/detail/error_msg.h>
#include <cstdarg>
#include <exception>

namespace base { 

	class _BASE_API exception : public std::exception
	{
	public:
		exception();
		exception(const char* fmt, ...);
		exception(const wchar_t* fmt, ...);
		virtual ~exception();
		virtual const char* what() const;

	protected:
		typedef exception_detail::error_msg<char>    error_msg;
		typedef exception_detail::error_msg<wchar_t> error_wmsg;

		error_msg  get_format_string(const char* fmt, ...) const;
		error_msg  get_format_string(const char* fmt, va_list argsList) const;
		error_wmsg get_format_string(const wchar_t* fmt, ...) const;
		error_wmsg get_format_string(const wchar_t* fmt, va_list argsList) const;
#pragma warning(suppress:4251)
		error_msg  what_;
	};

}
