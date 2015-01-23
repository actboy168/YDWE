#pragma once

#include <base/config.h>
#include <system_error>

namespace base { namespace win {
	class windows_category : public std::error_category
	{
	public:
		virtual const char* name() const;
		virtual std::string message(int error_code) const;
		virtual std::error_condition default_error_condition(int error_code) const;
	};

	_BASE_API std::string error_message();
	_BASE_API std::string error_message(int error_code);
}}
