#include <ydwe/exception/windows_exception.h>
#include <ydwe/exception/detail/windows_error_string.h>


_BASE_BEGIN

	windows_exception::windows_exception(const wchar_t* reason, uint32_t error_code)
		: exception(L"%s : %s", reason, error_string_ = exception_detail::windows_error_string::create(error_code))
		, error_code_(error_code)
	{ }

	windows_exception::~windows_exception()
	{
		exception_detail::windows_error_string::release(error_string_);
	}

	uint32_t windows_exception::error_code() const
	{
		return error_code_;
	}

_BASE_END
