#include <base/thread/thread_exception.h>
#include <Windows.h>

namespace base
{
	thread_exception::thread_exception()
		: std::system_error(0, std::system_category())
	{ }

	thread_exception::thread_exception(int sys_error_code)
		: std::system_error(sys_error_code, std::system_category())
	{ }

#if _MSC_VER <= 1600
	thread_exception::thread_exception(std::generic_errno ev, const char* what_arg)
#else			  
	thread_exception::thread_exception(std::errc ev, const char* what_arg)
#endif
		: std::system_error(std::error_code((int)ev, std::system_category()), what_arg)
	{ }

#if _MSC_VER <= 1600
	thread_exception::thread_exception(std::generic_errno ev, const std::string& what_arg)
#else																		   
	thread_exception::thread_exception(std::errc ev, const std::string& what_arg)
#endif
		: std::system_error(std::error_code((int)ev, std::system_category()), what_arg)
	{ }

	thread_exception::~thread_exception() throw()
	{ }

	int thread_exception::native_error() const
	{
		return code().value();
	}

	thread_resource_error::thread_resource_error()
		: thread_exception(::GetLastError())
	{ }

#if _MSC_VER <= 1600
	thread_resource_error::thread_resource_error(std::generic_errno ev, const char* what_arg)  
#else	
	thread_resource_error::thread_resource_error(std::errc ev, const char* what_arg)
#endif
		: thread_exception(ev, what_arg)
	{ }

#if _MSC_VER <= 1600
	thread_resource_error::thread_resource_error(std::generic_errno ev, const std::string& what_arg)
#else	
	thread_resource_error::thread_resource_error(std::errc ev, const std::string& what_arg)
#endif
		: thread_exception(ev, what_arg)
	{ }

	thread_resource_error::~thread_resource_error() throw()
	{ }
}
