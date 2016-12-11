#pragma once

#include <base/config.h>
#include <exception>
#include <system_error>

namespace base
{
#pragma warning(push)
#pragma warning(disable: 4275)
	class _BASE_API thread_exception
		: public std::system_error
	{
	public:
		thread_exception();
		thread_exception(int sys_error_code);
#if _MSC_VER <= 1600
		thread_exception(std::generic_errno ev, const char* what_arg);
		thread_exception(std::generic_errno ev, const std::string& what_arg);  
#else
		thread_exception(std::errc ev, const char* what_arg);
		thread_exception(std::errc ev, const std::string& what_arg);
#endif
		~thread_exception() throw();
		int native_error() const;
	};

	class _BASE_API thread_resource_error
		: public thread_exception
	{
	public:
		thread_resource_error();
#if _MSC_VER <= 1600
		thread_resource_error(std::generic_errno ev, const char* what_arg);
		thread_resource_error(std::generic_errno ev, const std::string& what_arg);
#else																
		thread_resource_error(std::errc ev, const char* what_arg);
		thread_resource_error(std::errc ev, const std::string& what_arg);
#endif
		~thread_resource_error() throw();
	};
#pragma warning(pop)
}
