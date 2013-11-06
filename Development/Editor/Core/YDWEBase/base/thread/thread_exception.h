#pragma once

#include <base/config.h>
#include <exception>
#include <system_error>

namespace base
{
	class _BASE_API thread_exception
		: public std::system_error
	{
	public:
		thread_exception();
		thread_exception(int sys_error_code);
		thread_exception(int ev, const char* what_arg);
		thread_exception(int ev, const std::string& what_arg);
		~thread_exception() throw();
		int native_error() const;
	};

	class _BASE_API thread_resource_error
		: public thread_exception
	{
	public:
		thread_resource_error();
		thread_resource_error(int ev, const char* what_arg);
		thread_resource_error(int ev, const std::string& what_arg);
		~thread_resource_error() throw();
	};
}
