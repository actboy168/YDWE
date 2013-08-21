#pragma once

#include <ydwe/config.h>
#include <ydwe/exception/system_exception.h>
#include <Windows.h>
#include <system_error>

_BASE_BEGIN

class _BASE_API windows_exception : public system_exception
{
public:
	windows_exception(const char* reason = nullptr, int error_code = ::GetLastError());
};

_BASE_END 
