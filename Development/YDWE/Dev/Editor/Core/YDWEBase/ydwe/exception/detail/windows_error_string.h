#pragma once

#include <ydwe/config.h>
#include <cstdint>

_BASE_BEGIN 

namespace exception_detail {

	struct windows_error_string
	{
	public:
		static wchar_t* create(uint32_t error_code);
		static void release(wchar_t* str);

	private:
		static const wchar_t* default_error_string_fmt;
		static uint32_t system_error_string(uint32_t error_code, wchar_t** buffer_ptr);
		static uint32_t default_error_string(uint32_t error_code, wchar_t** buffer_ptr);
	};
}

_BASE_END
