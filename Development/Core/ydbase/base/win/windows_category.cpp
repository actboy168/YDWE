#include <base/win/windows_category.h>
#include <base/exception/detail/error_msg.h>
#include <base/util/string_view.h>
#include <base/util/unicode.h>
#include <Windows.h>
#include <sstream>	
#include <system_error>

namespace base { namespace win {

	const char* windows_category::name() const
	{ 
		return "windows"; 
	}

	std::string windows_category::message(int error_code) const
	{
		exception_detail::error_msg<wchar_t> buffer;
		unsigned long result = ::FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  
			reinterpret_cast<LPWSTR>(buffer.get()), 
			0,
			NULL);

		if ((result == 0) || (!buffer))
		{
			std::ostringstream os;
			os << "Unable to get an error message for error code: " << error_code << ".";
			return os.str();
		}

		std::wstring_view str(buffer.c_str());
		while (str.size() && ((str.back() == L'\n') || (str.back() == L'\r')))
		{
			str.remove_suffix(1);
		}

		return std::move(w2u(str, conv_method::replace | '?'));
	}

	std::error_condition windows_category::default_error_condition(int error_code) const
	{
		const std::error_condition err_condition = std::system_category().default_error_condition(error_code);
		const std::string err_condition_msg = err_condition.message();
		if (_stricmp(err_condition_msg.c_str(), "unknown error") != 0)
		{
			return err_condition;
		}

		return std::error_condition(error_code, *this);
	}

	std::string error_message()
	{
		return error_message(GetLastError());
	}

	std::string error_message(int error_code)
	{
		static windows_category instance;
		return instance.message(error_code);
	}
}}
