#include <ydwe/exception/windows_exception.h>
#include <ydwe/exception/detail/windows_category.h>

_BASE_BEGIN

namespace exception_detail {
	const std::error_category& __cdecl windows_category()
	{
		static windows_category_impl instance;
		return instance;
	}
}

windows_exception::windows_exception(const char* reason, int error_code)
	: exception()
	, error_code_(error_code, exception_detail::windows_category())
{
	what_ = get_format_string("%s : %s", reason? reason: error_code_.category().name(), error_code_.message().c_str());
}

const std::error_code& windows_exception::error_code() const
{
	return error_code_;
}

_BASE_END
