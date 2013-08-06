#include <ydwe/config.h>
#include <system_error>

_BASE_BEGIN

namespace exception_detail
{
	std::error_code __cdecl create_error_code(unsigned long error_code);
}

_BASE_END
