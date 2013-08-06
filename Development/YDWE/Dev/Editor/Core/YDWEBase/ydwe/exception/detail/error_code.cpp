
#include <ydwe/exception/detail/error_code.h>
#include <ydwe/exception/detail/windows_category.h>

_BASE_BEGIN

namespace exception_detail
{
	std::error_code __cdecl create_error_code(unsigned long error_code)
	{
		return std::error_code((int)error_code, windows_category());
	}
}

_BASE_END
