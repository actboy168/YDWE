#include <base/exception/windows_exception.h>
#include <base/win/windows_category.h>

namespace base {

windows_exception::windows_exception(const char* reason, int error_code)
	: system_exception(std::error_code(error_code, win::windows_category()), reason)
{ }

}
