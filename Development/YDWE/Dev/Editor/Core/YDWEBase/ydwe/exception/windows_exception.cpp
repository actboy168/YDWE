#include <ydwe/exception/windows_exception.h>
#include <ydwe/exception/detail/windows_category.h>

_BASE_BEGIN

windows_exception::windows_exception(const char* reason, uint32_t error_code)
	: exception("%s : %s", reason, exception_detail::windows_category().message(error_code).c_str())
	, error_code_(error_code)
{ }

uint32_t windows_exception::error_code() const
{
	return error_code_;
}

_BASE_END
