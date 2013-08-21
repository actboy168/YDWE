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
	: system_exception(std::error_code(error_code, exception_detail::windows_category()), reason)
{ }

_BASE_END
