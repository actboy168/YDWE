#pragma once

#include <base/config.h>
#include <base/exception/system_exception.h>
#include <Windows.h>
#include <system_error>

namespace base {

class _BASE_API windows_exception : public system_exception
{
public:
	windows_exception(const char* reason = nullptr, int error_code = ::GetLastError());
};

} 
