#pragma once

#include <ydwe/config.h>
#include <ydwe/exception/exception.h>
#include <Windows.h>
#include <system_error>

_BASE_BEGIN

class _BASE_API windows_exception : public exception
{
public:
	windows_exception(const char* reason = nullptr, int error_code = ::GetLastError());
	const std::error_code& error_code() const;

protected:
#pragma warning(suppress:4251)
	std::error_code error_code_;
};

_BASE_END 
