#pragma once

#include <base/config.h>
#include <base/exception/exception.h>
#include <system_error>

namespace base {

class _BASE_API system_exception : public exception
{
public:
	system_exception(const std::error_code& ec, const char* reason = nullptr);
	const std::error_code& code() const;

protected:
#pragma warning(suppress:4251)
	std::error_code error_code_;
};

} 
