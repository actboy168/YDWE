#include <ydwe/exception/windows_exception.h>
#include <ydwe/exception/detail/windows_category.h>

_BASE_BEGIN

windows_exception::windows_exception(const char* reason, unsigned long error_code)
	: exception()
	, error_code_(exception_detail::create_error_code(error_code))
{
	what_ = get_format_string("%s : %s", reason, error_code_.message().c_str());
}

const std::error_code& windows_exception::error_code() const
{
	return error_code_;
}

_BASE_END
