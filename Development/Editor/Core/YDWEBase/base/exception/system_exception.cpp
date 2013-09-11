#include <base/exception/system_exception.h>

namespace base {

system_exception::system_exception(const std::error_code& ec, const char* reason)
	: error_code_(ec)
{ 
	what_ = exception::get_format_string("%s : %s", reason? reason: error_code_.category().name(), error_code_.message().c_str());
}

const std::error_code& system_exception::code() const
{
	return error_code_;
}

}
