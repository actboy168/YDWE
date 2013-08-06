#pragma once

#include <ydwe/config.h>
#include <ydwe/exception/exception.h>
#include <Windows.h>
#include <cstdint>

_BASE_BEGIN

class _BASE_API windows_exception : public exception
{
public:
	windows_exception(const char* reason = "windows exception", uint32_t error_code = ::GetLastError());
	uint32_t error_code() const;

protected:
	uint32_t error_code_;
};

_BASE_END 
