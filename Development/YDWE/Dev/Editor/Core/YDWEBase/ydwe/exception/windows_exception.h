#pragma once

#include <ydwe/config.h>
#include <ydwe/exception/exception.h>
#include <Windows.h>
#include <cstdint>

_BASE_BEGIN

class _BASE_API windows_exception : public exception
{
public:
	windows_exception(const wchar_t* reason = L"windows exception", uint32_t error_code = ::GetLastError());
	virtual ~windows_exception();
	uint32_t error_code() const;

protected:
	wchar_t* error_string_;
	uint32_t error_code_;
};

_BASE_END 
